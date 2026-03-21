#pragma once

#include <string>

namespace params
{
    struct BuildingParams
    {
        double min_building_area = 150.0;
        double max_aspect_ratio = 5.0;
        size_t min_contour_points = 3;
        size_t max_members = 5000;
        size_t min_members = 50;
        size_t min_fill_rate = 60;
    };

    // パラメータファイルを読み込む
    BuildingParams loadParams(const std::string &filepath);
}