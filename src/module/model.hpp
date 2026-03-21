#pragma once

#include <vector>

namespace model
{

    // 点の属性を表す
    // 1: 河川, 2: 地面, 3: 樹木, 4: 建物
    enum class TYPE
    {
        VOID = 0, // 存在しない点
        GROUND = 1,
        RIVER = 2,
        BUILDING = 3,
        TREE = 4,
        UNDETERMINED = 5, // 未判定点
        MISSING = 6,      // 欠損点
    };

    // 読み込まれる点データを表すため
    struct Point
    {
        int x;
        int y;
        double z;
        TYPE attr;
        bool visited = false;
    };

    struct Point3D
    {
        double x;
        double y;
        double z;
    };

    // 実際のxy座標ではなく、グリッド上のインデックスを表す
    struct Index
    {
        int x;
        int y;
    };

    class Grid
    {
    public:
        Grid(int xmin, int ymin, int x_size, int y_size);

        // 存在しない点のときはnullptrを返す
        Point *at(Index index);

        // 隣接セルのインデックスを返す
        std::vector<Index> neighbors(const Index &index);

        // Set or overwrite a point at (x,y).
        void setPoint(Point p);

        int x_size() const;
        int y_size() const;
        int x_min() const;
        int y_min() const;
        int x_max() const;
        int y_max() const;

        bool inBounds(Index index) const;

    private:
        int x_min_;
        int y_min_;
        int x_size_;
        int y_size_;

        // stored as (x, y)
        std::vector<std::vector<Point>> cells_;
    };

}
