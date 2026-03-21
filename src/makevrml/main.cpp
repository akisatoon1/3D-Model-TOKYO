#include "algo.hpp"
#include "input.hpp"
#include "log.hpp"
#include "output.hpp"
#include "model.hpp"
#include "params.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    std::string input_path;
    std::string log_path = "log.txt";
    std::string out_path = "out.wrl";
    std::string floor_path = "building_floors.dat";
    std::string param_path;
    int precision = 6;

    // コマンドライン引数の解析をする
    const std::string usage = "Usage: " + std::string(argv[0]) + " --input <path> [--out-vrml <path>] [--out-floor <path>] [--log <path>] [--param <path>]\n";

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc)
        {
            input_path = argv[++i];
        }
        else if (arg == "--out-vrml" && i + 1 < argc)
        {
            out_path = argv[++i];
        }
        else if (arg == "--out-floor" && i + 1 < argc)
        {
            floor_path = argv[++i];
        }
        else if (arg == "--log" && i + 1 < argc)
        {
            log_path = argv[++i];
        }
        else if (arg == "--param" && i + 1 < argc)
        {
            param_path = argv[++i];
        }
        else
        {
            std::cerr << "Unknown or incomplete arg: " << arg << "\n";
            std::cerr << usage;
            return 1;
        }
    }

    if (input_path.empty())
    {
        std::cerr << usage;
        return 1;
    }

    // ログファイルを初期化する
    if (!mylog::init_log(log_path))
    {
        std::cerr << "Failed to initialize log file\n";
        return 1;
    }

    // パラメータファイルが指定されている場合は読み込む
    if (!param_path.empty())
    {
        try
        {
            params::BuildingParams params = params::loadParams(param_path);
            algo::setParams(params);
            std::cout << "Loaded parameters from: " << param_path << "\n";
            std::cout << "  min_building_area: " << params.min_building_area << "\n";
            std::cout << "  max_aspect_ratio: " << params.max_aspect_ratio << "\n";
            std::cout << "  min_contour_points: " << params.min_contour_points << "\n";
            std::cout << "  max_members: " << params.max_members << "\n";
            std::cout << "  min_members: " << params.min_members << "\n";
            std::cout << "  min_fill_rate: " << params.min_fill_rate << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to load parameters: " << e.what() << "\n";
            return 1;
        }
    }
    else
    {
        std::cout << "Using default parameters\n";
    }

    // ファイルから読み込んだデータをグリッドに構築する
    std::cout << "Loading input data from: " << input_path << "\n";
    model::Grid grid = input::parseInput(input_path);

    // アルゴリズムで建物をグループに分ける
    std::cout << "Classifying buildings...\n";
    std::vector<algo::Cuboid> cuboids;
    std::vector<algo::ComplexBuilding> complex_buildings;
    algo::BuildingStats stats = algo::classfyBuildings(grid, cuboids, complex_buildings);

    // Cuboidをポリゴンデータに変換する
    std::cout << "Converting cuboids to VRML polygons...\n";
    std::vector<output::VrmlPolygon> polygons;
    polygons.reserve(cuboids.size() * 2);
    for (const auto &cuboid : cuboids)
    {
        auto cuboid_polygons = output::cuboidToPolygons(cuboid);
        for (const auto &poly : cuboid_polygons)
        {
            polygons.push_back(poly);
        }
    }

    // ComplexBuildingをポリゴンデータに変換する
    std::cout << "Converting complex buildings to VRML polygons...\n";
    for (const auto &complex_building : complex_buildings)
    {
        polygons.push_back(output::complexBuildingToPolygon(complex_building, stats));
    }

    // vrmlをファイルに出力する
    std::cout << "Writing VRML to: " << out_path << "\n";
    if (!output::writeVrml(out_path, polygons, precision))
    {
        std::cerr << "Failed to write VRML to " << out_path << "\n";
        return 1;
    }

    // 建物の足の点を別ファイルに出力する
    std::cout << "Writing building floors to: " << floor_path << "\n";
    if (!output::writeBuildingFloor(floor_path, cuboids, precision))
    {
        std::cerr << "Failed to write building floors to " << floor_path << "\n";
        return 1;
    }

    // メタデータを標準出力する
    std::cout << "\n=== Statistics ===\n";
    std::cout << "Grid bounds: x[" << grid.x_min() << ", " << grid.x_max() << "], y[" << grid.y_min() << ", " << grid.y_max() << "]\n";
    std::cout << "Total cuboids found: " << cuboids.size() << "\n";
    std::cout << "Exported cuboids:    " << cuboids.size() << "\n";
    std::cout << "Total complex buildings found: " << complex_buildings.size() << "\n";
    std::cout << "Exported complex buildings:    " << complex_buildings.size() << "\n";
    std::cout << "Total points before processing: " << stats.total_points_before << "\n";
    std::cout << "Total points after processing:  " << stats.total_points_after << "\n";
    std::cout << "Point reduction: " << (stats.total_points_before - stats.total_points_after) << " points\n";
    if (stats.total_points_before > 0)
    {
        double reduction_rate = 100.0 * (stats.total_points_before - stats.total_points_after) / stats.total_points_before;
        std::cout << "Reduction rate: " << reduction_rate << "%\n";
    }

    return 0;
}
