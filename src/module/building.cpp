#include "building.hpp"
#include "log.hpp"

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>

namespace building
{
    // BFSで連結成分を収集する
    static std::vector<model::Index> getConnected(model::Grid &grid, const model::Index &start)
    {
        std::vector<model::Index> connected;
        model::Point *startp = grid.at(start);
        if (!startp)
            return connected;
        if (startp->visited)
            return connected;

        std::queue<model::Index> q;
        startp->visited = true;
        q.push(start);

        while (!q.empty())
        {
            model::Index cur = q.front();
            q.pop();
            connected.push_back(cur);

            auto neighbors = grid.neighbors(cur);
            for (const auto &nidx : neighbors)
            {
                model::Point *np = grid.at(nidx);
                if (!np)
                    continue;
                if (np->visited)
                    continue;
                if (np->attr == model::TYPE::BUILDING)
                {
                    np->visited = true;
                    q.push(nidx);
                }
            }
        }
        return connected;
    }

    // コンストラクタ
    Building::Building(std::vector<model::Index> members_, model::Grid &grid)
        : members(std::move(members_))
    {
        validate_members();
        extractContour(grid);
    }

    size_t Building::memberCount() const
    {
        return members.size();
    }

    std::vector<model::Index> Building::getMembers()
    {
        return members;
    }

    std::vector<model::Index> Building::getContour()
    {
        return contour;
    }

    // membersのvalidation
    void Building::validate_members()
    {
        if (members.empty())
        {
            throw std::runtime_error("validate_members: warning: members is empty\n");
        }
    }

    void Building::extractContour(model::Grid &grid)
    {
        contour.clear();
        for (const auto &idx : members)
        {
            auto neighbors = grid.neighbors(idx);
            for (const auto &nidx : neighbors)
            {
                model::Point *np = grid.at(nidx);
                // 隣接セルが存在しないか建物でない場合は輪郭点
                if (!np || np->attr != model::TYPE::BUILDING)
                {
                    contour.push_back(idx);
                    break;
                }
            }
        }
    }

    double Building::computeFloorZ(model::Grid &grid)
    {
        // 輪郭点に隣接する地面のz座標の最小値を床の高さとする
        double min_z = std::numeric_limits<double>::infinity();
        for (const auto &idx : contour)
        {
            auto neighbors = grid.neighbors(idx);
            for (const auto &nidx : neighbors)
            {
                model::Point *np = grid.at(nidx);
                if (!np)
                    continue;
                if (np->attr != model::TYPE::GROUND)
                    continue;

                if (np->z < min_z)
                {
                    min_z = np->z;
                }
            }
        }

        if (min_z == std::numeric_limits<double>::infinity())
        {
            throw std::runtime_error("computeFloorZ: warning: no adjacent ground points for building (floor_z unset)\n");
        }
        else
        {
            return min_z;
        }
    }

    double Building::computeTopZ(model::Grid &grid)
    {
        // z座標の最大値を天井の高さとする
        double max_z = -std::numeric_limits<double>::infinity();
        for (const auto &idx : members)
        {
            model::Point *p = grid.at(idx);
            if (p && p->z > max_z)
            {
                max_z = p->z;
            }
        }

        return max_z;
    }

    // 連結成分を収集してBuildingオブジェクトのベクタを返す
    std::vector<Building> collectBuildings(model::Grid &grid)
    {
        std::vector<Building> buildings;

        // すべてのセルを走査し、未訪問の建物セルを見つけたら連結成分を収集する
        for (int x = 0; x < grid.x_size(); ++x)
        {
            for (int y = 0; y < grid.y_size(); ++y)
            {
                model::Index idx{x, y};
                model::Point *point = grid.at(idx);
                if (!point)
                    continue;
                if (point->visited)
                    continue;

                // この点を始点として連結成分を収集する
                if (point->attr == model::TYPE::BUILDING)
                {
                    std::vector<model::Index> connected = getConnected(grid, idx);

                    try
                    {
                        Building b(connected, grid);
                        buildings.push_back(b);
                    }
                    catch (const std::runtime_error &e)
                    {
                        mylog::output_log(e.what());
                    }
                }
                else
                {
                    // 非建物セルは訪問済みにしてスキップ
                    point->visited = true;
                }
            }
        }

        return buildings;
    }

} // namespace building