#pragma once

#include "model.hpp"
#include "params.hpp"
#include "building.hpp"

#include <array>
#include <utility>
#include <vector>
#include <opencv2/core/types.hpp>

namespace algo
{
    struct Rectangle
    {
        Rectangle(cv::RotatedRect &rr);
        std::pair<double, double> Corner(int index) const;
        std::array<std::pair<double, double>, 4> corners;
    };

    // 直方体を表す
    struct Cuboid
    {
        Cuboid(building::Building &b, model::Grid &grid, Rectangle rect);

        Rectangle rect; // 長方形の4点の座標 (x, y)
        double floor_z; // 床の高さ
        double top_z;   // 天井の高さ

        // 建物っぽくないものを除くため
        void validateRect();
    };

    // 複雑な建物を表す
    struct ComplexBuilding
    {
        ComplexBuilding(building::Building &b, model::Grid &grid);
        size_t verticesCount();

        std::vector<model::Point3D> vertices;
        std::vector<model::Point3D> contour;
        double floor_z;
    };

    // 統計データ構造体
    struct BuildingStats
    {
        size_t total_points_before = 0; // 処理前の建物の点の合計数
        size_t total_points_after = 0;  // 処理後の建物の点の合計数
    };

    // 連結成分でグループ分けして、それぞれを直方体と複雑な建物に分ける
    BuildingStats classfyBuildings(model::Grid &grid, std::vector<Cuboid> &cuboids, std::vector<ComplexBuilding> &complex_buildings);

    // パラメータを設定する関数
    void setParams(const params::BuildingParams &params);

}
