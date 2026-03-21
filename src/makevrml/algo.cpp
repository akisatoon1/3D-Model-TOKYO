#include "algo.hpp"
#include "log.hpp"
#include "output.hpp"
#include "model.hpp"
#include "params.hpp"
#include "building.hpp"

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>

#include <opencv2/imgproc.hpp>
#include "opencv2/core/mat.hpp"
#include "opencv2/core/mat.inl.hpp"
#include "opencv2/core/traits.hpp"
#include "opencv2/core/types.hpp"

namespace algo
{
    // グローバル変数として保持（デフォルト値）
    static params::BuildingParams g_params;

    void setParams(const params::BuildingParams &params)
    {
        g_params = params;
    }

    Rectangle::Rectangle(cv::RotatedRect &rr)
    {
        cv::Point2f pts[4];
        rr.points(pts);

        for (int i = 0; i < 4; ++i)
        {
            corners[i].first = static_cast<double>(pts[i].x);
            corners[i].second = static_cast<double>(pts[i].y);
        }
    }

    std::pair<double, double> Rectangle::Corner(int index) const
    {
        if (index < 0 || index >= 4)
        {
            throw std::out_of_range("Rectangle::Corner: index out of range");
        }
        return corners[index];
    }

    // 長方形の4点を計算する独立した関数
    Rectangle computeRectangle(const std::vector<model::Index> &contour, model::Grid &grid)
    {
        // contour の座標を集める（world 座標は grid.at(idx)->x/y に格納されている）
        std::vector<cv::Point2f> pts;
        pts.reserve(contour.size());
        for (const auto &idx : contour)
        {
            model::Point *p = grid.at(idx);
            if (!p)
                continue;
            pts.emplace_back(static_cast<float>(p->x), static_cast<float>(p->y));
        }

        if (pts.size() < g_params.min_contour_points)
        {
            throw std::runtime_error("computeRectangle: warning: insufficient contour points (" + std::to_string(pts.size()) + ")\n");
        }

        // OpenCV の最小外接矩形を利用
        cv::RotatedRect rr = cv::minAreaRect(pts);
        Rectangle rect(rr);
        return rect;
    }

    // Buildingのベクタを受け取り、CuboidとComplexBuildingに分類する
    BuildingStats classifyBuildingTypes(const std::vector<building::Building> &buildings, model::Grid &grid,
                                        std::vector<Cuboid> &cuboids, std::vector<ComplexBuilding> &complex_buildings)
    {
        BuildingStats stats;

        for (const auto &building_obj : buildings)
        {
            // constなbuildingから非constのコピーを作成
            building::Building b = building_obj;
            stats.total_points_before += b.memberCount();

            // memberの数が少ない場合はスキップ
            if (b.memberCount() < g_params.min_members)
            {
                mylog::output_log("classifyBuildingTypes: info: building skipped due to insufficient members (" + std::to_string(b.memberCount()) + ")\n");
                continue;
            }

            try
            {
                // 長方形の面積を求めて、充填率を計算
                Rectangle rect = computeRectangle(b.getContour(), grid);
                double side1 = std::hypot(rect.Corner(0).first - rect.Corner(1).first, rect.Corner(0).second - rect.Corner(1).second);
                double side2 = std::hypot(rect.Corner(1).first - rect.Corner(2).first, rect.Corner(1).second - rect.Corner(2).second);
                double rect_area = side1 * side2;
                double fill_rate = (static_cast<double>(b.memberCount()) / rect_area) * 100.0;

                // 点が多すぎない、かつ充填率が高いものを直方体で表す
                if (b.memberCount() < g_params.max_members && fill_rate >= g_params.min_fill_rate)
                {
                    Cuboid cuboid(b, grid, rect);
                    cuboids.push_back(cuboid);

                    // 直方体の場合は8点(頂点)で表現するが、
                    // 床点は地面構築の方でも統計が取られているので
                    // 上の4点のみ加算する
                    stats.total_points_after += 4;
                }
                else
                {
                    // 複雑な建物として扱う
                    ComplexBuilding cb(b, grid);
                    complex_buildings.push_back(cb);

                    // 複雑な建物の点数を加算
                    stats.total_points_after += cb.verticesCount();
                }
            }
            catch (const std::runtime_error &e)
            {
                mylog::output_log(e.what());
            }
        }

        return stats;
    }

    // 連結成分でグループ分けして、それぞれを直方体と複雑な建物に分ける
    BuildingStats classfyBuildings(model::Grid &grid, std::vector<Cuboid> &cuboids, std::vector<ComplexBuilding> &complex_buildings)
    {
        // 1. 連結成分を収集してBuildingオブジェクトを作成
        std::vector<building::Building> buildings = building::collectBuildings(grid);

        // 2. BuildingをCuboidとComplexBuildingに分類
        return classifyBuildingTypes(buildings, grid, cuboids, complex_buildings);
    }

    Cuboid::Cuboid(building::Building &b, model::Grid &grid, Rectangle rect)
        : rect(rect)
    {
        floor_z = b.computeFloorZ(grid);
        top_z = b.computeTopZ(grid);
        validateRect();
    }

    void Cuboid::validateRect()
    {
        // 長方形の2辺の長さを計算する
        double side1 = std::hypot(rect.Corner(0).first - rect.Corner(1).first, rect.Corner(0).second - rect.Corner(1).second);
        double side2 = std::hypot(rect.Corner(1).first - rect.Corner(2).first, rect.Corner(1).second - rect.Corner(2).second);

        // 長方形の面積を計算する
        double area = side1 * side2;

        // 長方形の面積が基準より小さい場合は表現しないため
        if (area < g_params.min_building_area)
        {
            throw std::runtime_error("validateRect: warning: rectangle area too small\n");
        }

        // 細長すぎる建物は表現しないため
        double ratio = side1 > side2 ? side1 / side2 : side2 / side1;
        if (ratio > g_params.max_aspect_ratio)
        {
            throw std::runtime_error("validateRect: warning: rectangle aspect ratio too large\n");
        }
    }

    // TODO: 未使用のメンバがある
    ComplexBuilding::ComplexBuilding(building::Building &b, model::Grid &grid)
    {
        // vertticesを構築
        for (const auto &midx : b.getMembers())
        {
            model::Point *mp = grid.at(midx);
            if (mp)
            {
                model::Point3D v{static_cast<double>(mp->x), static_cast<double>(mp->y), mp->z};
                vertices.push_back(v);
            }
        }

        // contourを構築
        double floor_z = b.computeFloorZ(grid);
        for (const auto &midx : b.getContour())
        {
            model::Point *mp = grid.at(midx);
            if (mp)
            {
                model::Point3D v{static_cast<double>(mp->x), static_cast<double>(mp->y), mp->z};
                contour.push_back(v);
            }
        }

        floor_z = b.computeFloorZ(grid);
    }

    size_t ComplexBuilding::verticesCount()
    {
        return vertices.size();
    }

} // namespace algo
