#include "algo.hpp"
#include "log.hpp"
#include "output.hpp"
#include "model.hpp"

#include <array>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "delaunator.hpp"

namespace output
{

    // 直方体の8頂点を計算する
    // bottom 0..3 then top 4..7
    std::array<model::Point3D, 8> calculateCuboidVertices(const algo::Cuboid &cuboid)
    {
        std::array<model::Point3D, 8> vertices;
        for (int i = 0; i < 4; ++i)
        {
            std::pair<double, double> corner = cuboid.rect.Corner(i);
            vertices[i] = model::Point3D{corner.first, corner.second, cuboid.floor_z};
            vertices[i + 4] = model::Point3D{corner.first, corner.second, cuboid.top_z};
        }
        return vertices;
    }

    // 直方体の表面のポリゴンを計算する
    VrmlPolygon cuboidFrontSurface(std::vector<model::Point3D> &vertices)
    {
        VrmlPolygon polygon;

        for (const auto &v : vertices)
        {
            polygon.vertices.push_back(v);
        }

        // 6面のインデックスを設定
        polygon.faces = {
            {3, 2, 1, 0, -1}, // bottom
            {5, 6, 7, 4, -1}, // top
            {0, 1, 5, 4, -1}, // side1
            {2, 6, 5, 1, -1}, // side 2
            {3, 7, 6, 2, -1}, // side 3
            {0, 4, 7, 3, -1}, // side 4
        };

        return polygon;
    }

    std::vector<VrmlPolygon> cuboidToPolygons(const algo::Cuboid &cuboid)
    {
        std::vector<model::Point3D> vertices;

        // 頂点を計算して追加
        auto basic_vertices = calculateCuboidVertices(cuboid);
        for (const auto &v : basic_vertices)
        {
            vertices.push_back(v);
        }

        std::vector<VrmlPolygon> polygons;
        polygons.push_back(cuboidFrontSurface(vertices));
        return polygons;
    }

    // 簡易アルファシェイプのため
    bool filter2DTriangle(const model::Point3D &p0, const model::Point3D &p1, const model::Point3D &p2)
    {
        // 閾値の長さを超える辺を持つならばfalseを返す

        const double max_edge_length = 2.0;

        auto distance2D = [](const model::Point3D &a, const model::Point3D &b)
        {
            double dx = a.x - b.x;
            double dy = a.y - b.y;
            return std::sqrt(dx * dx + dy * dy);
        };

        double d01 = distance2D(p0, p1);
        double d12 = distance2D(p1, p2);
        double d20 = distance2D(p2, p0);

        bool result = (d01 <= max_edge_length && d12 <= max_edge_length && d20 <= max_edge_length);
        return result;
    }

    // 凹包を返す
    std::vector<std::vector<int>> concavehull(const std::vector<std::vector<int>> &faces)
    {
        // エッジの出現回数をカウント（辞書として使用）
        // エッジを (min_idx, max_idx) の形で正規化して格納
        std::map<std::pair<int, int>, int> edge_count;

        // 各三角形からエッジを抽出してカウント
        for (const auto &face : faces)
        {
            if (face.size() < 3)
                continue;

            // 三角形の3つのエッジを処理（最後の-1は除く）
            int num_vertices = 0;
            for (size_t i = 0; i < face.size(); ++i)
            {
                if (face[i] == -1)
                    break;
                ++num_vertices;
            }

            for (int i = 0; i < num_vertices; ++i)
            {
                int v1 = face[i];
                int v2 = face[(i + 1) % num_vertices];

                // エッジを正規化（小さい方を先に）
                auto edge = (v1 < v2) ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
                edge_count[edge]++;
            }
        }

        // 境界エッジを抽出（1回だけ出現するエッジ）
        std::vector<std::pair<int, int>> boundary_edges;
        for (const auto &entry : edge_count)
        {
            if (entry.second == 1)
            {
                boundary_edges.push_back(entry.first);
            }
        }

        if (boundary_edges.empty())
        {
            return {}; // 境界なし
        }

        // エッジを順序付けてパスを構築
        // 隣接リストを構築
        std::map<int, std::vector<int>> adjacency;
        for (const auto &edge : boundary_edges)
        {
            adjacency[edge.first].push_back(edge.second);
            adjacency[edge.second].push_back(edge.first);
        }

        std::vector<std::vector<int>> result;
        std::set<int> visited_vertices;

        // 各連結成分を処理
        for (const auto &edge : boundary_edges)
        {
            int start = edge.first;
            if (visited_vertices.count(start))
                continue;

            std::vector<int> path;
            int current = start;
            int prev = -1;

            // パスを辿る
            while (true)
            {
                path.push_back(current);
                visited_vertices.insert(current); // 現在地を訪問済みにする

                // 次の頂点を探す
                int next = -1;
                bool closed = false; // ループが閉じたかどうかのフラグ

                for (int neighbor : adjacency[current])
                {
                    if (neighbor == prev)
                        continue; // 来た道には戻らない

                    // 【修正点】: ゴール(start)に戻るか、未訪問の点だけを選ぶ
                    if (neighbor == start)
                    {
                        next = neighbor;
                        closed = true;
                        break; // ゴール到達
                    }

                    // まだ訪問していない点なら進める
                    // (グローバルな visited_vertices をチェックすることで、
                    //  自分自身のパスとの交差だけでなく、他のポリゴンへの誤進入も防げます)
                    if (visited_vertices.find(neighbor) == visited_vertices.end())
                    {
                        next = neighbor;
                        break;
                    }
                }

                if (closed)
                {
                    // 正常にループが閉じた
                    break;
                }

                if (next == -1)
                {
                    // 行き止まり、または既に訪問済みの点しか残っていない（交差発生）
                    // 無限ループを防ぐためここで打ち切る
                    break;
                }

                prev = current;
                current = next;
            }

            if (path.size() >= 3)
            {
                result.push_back(path);
            }
        }

        return result;
    }

    VrmlPolygon complexBuildingToPolygon(const algo::ComplexBuilding &complex_building, algo::BuildingStats &stats)
    {
        VrmlPolygon polygon;
        polygon.isSolid = false; // 表と裏考えるのだるい

        if (complex_building.vertices.empty())
        {
            mylog::output_log("complexBuildingToPolygon: warning: empty vertices\n");
            return polygon;
        }

        // 頂点座標をdelaunatorの形式に変換: x0, y0, x1, y1, ...
        std::vector<double> coords;

        for (const auto &pt3D : complex_building.vertices)
        {
            coords.push_back(pt3D.x);
            coords.push_back(pt3D.y);
        }

        // ドロネー三角形分割を実行
        delaunator::Delaunator d(coords);

        // 上面の頂点を追加
        for (const auto &pt3D : complex_building.vertices)
        {
            polygon.vertices.push_back(pt3D);
        }

        // 上面の三角形の面インデックスを生成
        for (std::size_t i = 0; i < d.triangles.size(); i += 3)
        {
            // triangel[i], triangle[i+1], triangle[i+2] が頂点インデックス
            int idx1, idx2, idx3;
            idx1 = static_cast<int>(d.triangles[i]);
            idx2 = static_cast<int>(d.triangles[i + 1]);
            idx3 = static_cast<int>(d.triangles[i + 2]);

            if (!filter2DTriangle(
                    polygon.vertices[idx1],
                    polygon.vertices[idx2],
                    polygon.vertices[idx3]))
            {
                continue;
            }

            polygon.faces.push_back({idx1, idx2, idx3, -1});
        }

        // 凹包で境界を抽出
        std::cout << "  Calculating concave hull for complex building...\n";
        std::vector<std::vector<int>> boundary_paths = concavehull(polygon.faces);

        // 統計情報を更新
        size_t boundary_vertex_count = 0;
        for (const auto &path : boundary_paths)
        {
            boundary_vertex_count += path.size();
        }
        stats.total_points_after += boundary_vertex_count;

        // 側面の面を追加
        const double floor_z = complex_building.floor_z;

        for (const auto &path : boundary_paths)
        {
            if (path.empty())
                continue;

            // 境界上の各頂点について床頂点を作成
            std::map<int, int> top_to_floor_idx;
            for (int top_idx : path)
            {
                if (top_to_floor_idx.count(top_idx))
                    continue;

                // 床の頂点を追加（xとyは同じ、zだけ変更）
                model::Point3D floor_vertex = polygon.vertices[top_idx];
                floor_vertex.z = floor_z;

                int floor_idx = static_cast<int>(polygon.vertices.size());
                polygon.vertices.push_back(floor_vertex);
                top_to_floor_idx[top_idx] = floor_idx;
            }

            // 境界の各エッジについて側面の四角形を作成
            for (size_t i = 0; i < path.size(); ++i)
            {
                int v1 = path[i];
                int v2 = path[(i + 1) % path.size()];

                int v1_floor = top_to_floor_idx[v1];
                int v2_floor = top_to_floor_idx[v2];

                // 側面の四角形: 上面の2点と床の2点
                polygon.faces.push_back({v1, v2, v2_floor, v1_floor, -1});
            }
        }

        return polygon;
    }

    bool writeVrml(const std::string &path, const std::vector<VrmlPolygon> &polygons, int precision)
    {
        std::ofstream ofs(path);
        if (!ofs)
        {
            mylog::output_log("writeVrml: failed to open output file: " + path + "\n");
            return false;
        }

        // TODO; what is precision
        if (precision < 0)
            precision = 6;
        ofs << std::fixed << std::setprecision(precision);

        // VRML header
        ofs << "#VRML V2.0 utf8\n\n";

        for (const auto &polygon : polygons)
        {
            ofs << "Shape {\n";
            ofs << "  appearance Appearance {\n";
            ofs << "    material Material {\n";
            ofs << "      diffuseColor " << polygon.color[0] << " " << polygon.color[1] << " " << polygon.color[2] << "\n";
            ofs << "    }\n";
            ofs << "  }\n";
            ofs << "  geometry IndexedFaceSet {\n";
            ofs << "    solid " << (polygon.isSolid ? "TRUE" : "FALSE") << "\n";
            ofs << "    coord Coordinate {\n";
            ofs << "      point [\n";

            // 頂点を出力
            for (size_t i = 0; i < polygon.vertices.size(); ++i)
            {
                const auto &v = polygon.vertices[i];
                ofs << "        ";
                // VRML座標系に変換: (X,Y,Z) = (x, z, -y)
                ofs << v.x << " " << v.z << " " << -v.y;
                if (i != polygon.vertices.size() - 1)
                    ofs << ",\n";
                else
                    ofs << "\n";
            }

            ofs << "      ]\n";
            ofs << "    }\n"; // coord

            // 面インデックスを出力
            ofs << "    coordIndex [\n";
            for (size_t i = 0; i < polygon.faces.size(); ++i)
            {
                ofs << "      ";
                for (size_t j = 0; j < polygon.faces[i].size(); ++j)
                {
                    ofs << polygon.faces[i][j];
                    if (j != polygon.faces[i].size() - 1)
                        ofs << ",";
                }
                if (i != polygon.faces.size() - 1)
                    ofs << ",\n";
                else
                    ofs << "\n";
            }
            ofs << "    ]\n";

            ofs << "  }\n"; // geometry
            ofs << "}\n\n"; // Shape
        }

        return true;
    }

    bool writeBuildingFloor(const std::string &path, const std::vector<algo::Cuboid> &cuboids, int precision)
    {
        std::ofstream ofs(path);
        if (!ofs)
        {
            mylog::output_log("writeBuildingFloor: failed to open output file: " + path + "\n");
            return false;
        }

        // TODO; what is precision
        if (precision < 0)
            precision = 6;
        ofs << std::fixed << std::setprecision(precision);

        // `x y z 1`という形式で各建物の足の点を出力する
        for (const auto &cuboid : cuboids)
        {
            for (int i = 0; i < 4; ++i)
            {
                std::pair<double, double> corner = cuboid.rect.Corner(i);

                double x = corner.first;
                double y = corner.second;
                double z = cuboid.floor_z;
                ofs << x << " " << y << " " << z << " 1" << "\n";
            }
        }

        return true;
    }
}
