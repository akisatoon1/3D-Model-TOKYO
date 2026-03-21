#include "input.hpp"
#include "model.hpp"
#include "log.hpp"

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace input
{
    int convertAttrToInt(model::TYPE attr)
    {
        switch (attr)
        {
        case model::TYPE::MISSING:
            return 0;
        case model::TYPE::GROUND:
            return 1;
        case model::TYPE::BUILDING:
            return 3;
        default:
            return -1; // Unknown attribute
        }
    }

    model::Grid parseInput(const std::string &path)
    {
        // open file
        std::ifstream ifs(path);
        if (!ifs.is_open())
        {

            std::cerr << "parseInput: failed to open input file: " + path << std::endl;
            std::exit(1);
        }

        // テキストから読み込んだ, 全ての点データを格納するため
        std::vector<model::Point> points;

        // 読み込んだテキストの行ごとに処理するため
        std::string line;
        std::size_t currentlineNo = 0;

        // gridの範囲を決定するために, 最小・最大のx,yを記録しておく
        int xmin = std::numeric_limits<int>::max();
        int xmax = std::numeric_limits<int>::min();
        int ymin = std::numeric_limits<int>::max();
        int ymax = std::numeric_limits<int>::min();

        while (std::getline(ifs, line))
        {
            ++currentlineNo;

            // 空白で区切られている行の内容を、それぞれの変数に格納するため
            std::istringstream ss(line);

            int x, y;
            double z;
            int attr_i;
            double x_tmp, y_tmp; // originalは小数なため

            if (!(ss >> x_tmp >> y_tmp >> z >> attr_i))
            {
                mylog::output_log("parseInput: warning: invalid/partial line " + std::to_string(currentlineNo) + ", skipping\n");
                continue;
            }

            // 小数を切り捨てる
            x = static_cast<int>(x_tmp);
            y = static_cast<int>(y_tmp);

            // 属性値をenumに変換する
            model::TYPE attr;
            switch (attr_i)
            {
            case 0:
                attr = model::TYPE::MISSING;
                break;
            case 1:
                attr = model::TYPE::GROUND;
                break;
            case 3:
                attr = model::TYPE::BUILDING;
                break;
            default:
                mylog::output_log("parseInput: warning: unknown attr value " + std::to_string(attr_i) + " on line " + std::to_string(currentlineNo) + ", treating as EMPTY\n");
                attr = model::TYPE::VOID;
                break;
            }

            model::Point p = {x, y, z, attr, false};
            points.push_back(p);

            if (x < xmin)
                xmin = x;
            if (x > xmax)
                xmax = x;
            if (y < ymin)
                ymin = y;
            if (y > ymax)
                ymax = y;
        }

        // gridのサイズを計算する
        int x_size = xmax - xmin + 1;
        int y_size = ymax - ymin + 1;
        if (x_size <= 0 || y_size <= 0)
        {
            std::cerr << "parseInput: computed invalid grid dimensions\n";
            std::exit(1);
        }

        // gridを作成し, 点データを追加する
        model::Grid grid(xmin, ymin, x_size, y_size);
        for (const auto &p : points)
        {
            grid.setPoint(p);
        }

        // close file
        ifs.close();

        return grid;
    }
}