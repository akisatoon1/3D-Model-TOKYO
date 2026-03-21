#include "model.hpp"

#include <cmath>
#include <utility>

namespace model
{

    Grid::Grid(int xmin, int ymin, int x_size, int y_size)
        : x_min_(xmin), y_min_(ymin), x_size_(x_size), y_size_(y_size), cells_(x_size, std::vector<Point>(y_size))
    {
        // Initialize cells with VOID points
        for (int x = 0; x < x_size; ++x)
        {
            for (int y = 0; y < y_size; ++y)
            {
                cells_[x][y] = Point{x + xmin, y + ymin, 0.0, TYPE::VOID, false};
            }
        }
    }

    Point *Grid::at(Index index)
    {
        int x_idx = index.x;
        int y_idx = index.y;
        if (!inBounds(index))
        {
            return nullptr;
        }
        Point &p = cells_[x_idx][y_idx];
        if (p.attr == TYPE::VOID)
        {
            return nullptr;
        }
        return &p;
    }

    std::vector<Index> Grid::neighbors(const Index &index)
    {
        std::vector<Index> result;
        std::vector<std::pair<int, int>> deltas = {
            {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
        for (const auto &delta : deltas)
        {
            Index neighbor = {index.x + delta.first, index.y + delta.second};
            if (inBounds(neighbor))
            {
                result.push_back(neighbor);
            }
        }
        return result;
    }

    void Grid::setPoint(Point p)
    {
        int x = static_cast<int>(std::trunc(p.x));
        int y = static_cast<int>(std::trunc(p.y));
        int x_idx = x - x_min_;
        int y_idx = y - y_min_;
        cells_[x_idx][y_idx] = p;
    }

    int Grid::x_size() const
    {
        return x_size_;
    }

    int Grid::y_size() const
    {
        return y_size_;
    }

    int Grid::x_min() const
    {
        return x_min_;
    }

    int Grid::y_min() const
    {
        return y_min_;
    }

    int Grid::x_max() const
    {
        return x_min_ + x_size_ - 1;
    }

    int Grid::y_max() const
    {
        return y_min_ + y_size_ - 1;
    }

    bool Grid::inBounds(Index index) const
    {
        int x_idx = index.x;
        int y_idx = index.y;
        return !(x_idx < 0 || x_idx >= x_size_ || y_idx < 0 || y_idx >= y_size_);
    }
}