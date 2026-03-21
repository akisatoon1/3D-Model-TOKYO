#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace algo
{
    class Building;
    struct Cuboid;
    struct BuildingStats;
}

namespace output
{
    // 汎用的なVRMLポリゴンデータ
    struct VrmlPolygon
    {
        std::vector<model::Point3D> vertices;          // 各頂点 (x, y, z)
        std::vector<std::vector<int>> faces;           // 各面のインデックス (-1で終端)
        std::array<double, 3> color = {0.8, 0.6, 0.4}; // デフォルトの色 (RGB)
        bool isSolid = true;
    };

    // Cuboidをポリゴンデータに変換する
    std::vector<VrmlPolygon> cuboidToPolygons(const algo::Cuboid &cuboid);

    // ComplexBuildingをポリゴンデータに変換する
    VrmlPolygon complexBuildingToPolygon(const algo::ComplexBuilding &complex_building, algo::BuildingStats &stats);

    // ポリゴンデータをVRMLファイルに書き込む
    bool writeVrml(const std::string &path, const std::vector<VrmlPolygon> &polygons, int precision = 6);

    // 建物の足の点を別ファイルに出力する
    bool writeBuildingFloor(const std::string &path, const std::vector<algo::Cuboid> &cuboids, int precision = 6);
}
