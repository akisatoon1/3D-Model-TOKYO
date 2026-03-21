#pragma once

#include "model.hpp"

#include <vector>

namespace building
{
    class Building
    {
    public:
        Building(std::vector<model::Index> members, model::Grid &grid);

        size_t memberCount() const;

        std::vector<model::Index> getMembers();

        std::vector<model::Index> getContour();

        double computeFloorZ(model::Grid &grid);

        double computeTopZ(model::Grid &grid);

    private:
        std::vector<model::Index> members;
        std::vector<model::Index> contour;

        void validate_members();

        void extractContour(model::Grid &grid);
    };

    // 連結成分を収集してBuildingオブジェクトのベクタを返す
    std::vector<Building> collectBuildings(model::Grid &grid);

} // namespace building
