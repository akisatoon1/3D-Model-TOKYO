#include "model.hpp"
#include "input.hpp"

#include <iostream>
#include <fstream>

int KERNEL_SIZE = 3;

// 与えられたインデックスに対し、モルフォロジー変換の膨張処理をした属性値を返す
model::TYPE dilateAtIndex(model::Index idx, model::Grid &grid)
{
    // Indexに空のポイントが入っている場合は、TYPE EMPTYを返す
    model::Point *center = grid.at(idx);
    if (center == nullptr)
    {
        return model::TYPE::VOID;
    }

    // カーネルの半径を計算
    int radius = KERNEL_SIZE / 2;

    // カーネルの範囲でTYPE BUILDINGが一つでも含まれれば、TYPE BUILDING
    bool hasBuildingInKernel = false;

    // カーネル範囲内を探索 (KERNEL_SIZE * KERNEL_SIZE の正方行列)
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            model::Index kernelIdx = {idx.x + dx, idx.y + dy};
            if (!grid.inBounds(kernelIdx))
            {
                // 範囲外のインデックスは無視
                continue;
            }

            model::Point *p = grid.at(kernelIdx);
            if (p == nullptr)
            {
                // 空のポイントは無視
                continue;
            }

            // 一つでもBUILDINGがあれば、BUILDINGだと判定するため
            if (p->attr == model::TYPE::BUILDING)
            {
                hasBuildingInKernel = true;
                break;
            }
        }
    }

    // BUILDINGがあればBUILDING、なければGROUND
    if (hasBuildingInKernel)
    {
        return model::TYPE::BUILDING;
    }
    else
    {
        return model::TYPE::GROUND;
    }
}

// 与えられたインデックスに対し、モルフォロジー変換の縮小処理をした属性値を返す
model::TYPE erodeAtIndex(model::Index idx, model::Grid &grid)
{
    // Indexに空のポイントが入っている場合は、TYPE EMPTYを返す
    model::Point *center = grid.at(idx);
    if (center == nullptr)
    {
        return model::TYPE::VOID;
    }

    // カーネルの半径を計算
    int radius = KERNEL_SIZE / 2;

    // カーネルの範囲でTYPE GROUNDが一つでも含まれれば、TYPE GROUND
    bool hasGroundInKernel = false;

    // カーネル範囲内を探索 (KERNEL_SIZE * KERNEL_SIZE の正方行列)
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            model::Index kernelIdx = {idx.x + dx, idx.y + dy};
            if (!grid.inBounds(kernelIdx))
            {
                // 範囲外のインデックスは無視
                continue;
            }

            model::Point *p = grid.at(kernelIdx);
            if (p == nullptr)
            {
                // 空のポイントは無視
                continue;
            }

            // 一つでもGROUNDがあれば、GROUNDだと判定するため
            if (p->attr == model::TYPE::GROUND || p->attr == model::TYPE::RIVER)
            {
                hasGroundInKernel = true;
                break;
            }
        }
    }

    // GROUNDがあればGROUND、なければBUILDING
    if (hasGroundInKernel)
    {
        return model::TYPE::GROUND;
    }
    else
    {
        return model::TYPE::BUILDING;
    }
}

// 膨張処理
model::Grid dilate(model::Grid &grid)
{
    // 1. 同サイズの新しいgridを作る
    model::Grid newGrid(grid.x_min(), grid.y_min(), grid.x_size(), grid.y_size());

    // 2. カーネルをスライドさせてすべてのインデックスに対してerodeを行う
    for (int y = 0; y < grid.y_size(); y++)
    {
        for (int x = 0; x < grid.x_size(); x++)
        {
            model::Index idx = {x, y};
            model::Point *originalPoint = grid.at(idx);
            if (originalPoint == nullptr)
            {
                continue;
            }

            // 欠損点はそのままコピーする
            if (originalPoint->attr == model::TYPE::MISSING)
            {
                model::Point newPoint = *originalPoint;
                newGrid.setPoint(newPoint);
                continue;
            }

            // dilateを適用
            model::TYPE newAttr = dilateAtIndex(idx, grid);

            // 3. その属性をもった新しいpointをgridに挿入する
            model::Point newPoint = *originalPoint;
            newPoint.attr = newAttr;
            newGrid.setPoint(newPoint);
        }
    }

    return newGrid;
}

// 収縮処理
model::Grid erode(model::Grid &grid)
{
    // 1. 同サイズの新しいgridを作る
    model::Grid newGrid(grid.x_min(), grid.y_min(), grid.x_size(), grid.y_size());

    // 2. カーネルをスライドさせてすべてのインデックスに対してerodeを行う
    for (int y = 0; y < grid.y_size(); y++)
    {
        for (int x = 0; x < grid.x_size(); x++)
        {
            model::Index idx = {x, y};
            model::Point *originalPoint = grid.at(idx);
            if (originalPoint == nullptr)
            {
                continue;
            }

            // 欠損点はそのままコピーする
            if (originalPoint->attr == model::TYPE::MISSING)
            {
                model::Point newPoint = *originalPoint;
                newGrid.setPoint(newPoint);
                continue;
            }

            // erodeを適用
            model::TYPE newAttr = erodeAtIndex(idx, grid);

            // 3. その属性をもった新しいpointをgridに挿入する
            model::Point newPoint = *originalPoint;
            newPoint.attr = newAttr;
            newGrid.setPoint(newPoint);
        }
    }

    return newGrid;
}

model::Grid opening(model::Grid &grid)
{
    // 収縮 -> 膨張
    model::Grid erodedGrid = erode(grid);
    model::Grid openedGrid = dilate(erodedGrid);
    return openedGrid;
}

void outputGrid(model::Grid &grid, const std::string &filename)
{
    std::ofstream ofs(filename);
    if (!ofs)
    {
        std::cerr << "Error opening output file: " << filename << std::endl;
        return;
    }

    for (int y = 0; y < grid.y_size(); y++)
    {
        for (int x = 0; x < grid.x_size(); x++)
        {
            model::Index idx = {x, y};
            model::Point *p = grid.at(idx);
            if (p != nullptr)
            {
                ofs << p->x << " " << p->y << " " << p->z << " " << input::convertAttrToInt(p->attr) << "\n";
            }
        }
    }

    ofs.close();
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> [--size <kernel_size>]" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    // コマンドライン引数から--sizeオプションを解析
    for (int i = 3; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "--size")
        {
            if (i + 1 < argc)
            {
                KERNEL_SIZE = std::stoi(argv[i + 1]);
                i++; // 次の引数をスキップ
            }
            else
            {
                std::cerr << "Error: --size option requires a value" << std::endl;
                return 1;
            }
        }
    }

    // 入力データの読み込み
    std::cout << "Loading input data from " << inputFile << "..." << std::endl;
    model::Grid grid = input::parseInput(inputFile);

    // モルフォロジー変換の開閉処理
    std::cout << "Performing opening with kernel size " << KERNEL_SIZE << "..." << std::endl;
    model::Grid openedGrid = opening(grid);

    // 結果の出力
    std::cout << "Writing output data to " << outputFile << "..." << std::endl;
    outputGrid(openedGrid, outputFile);

    std::cout << "Done." << std::endl;
    return 0;
}
