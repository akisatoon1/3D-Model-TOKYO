#pragma once

#include <string>

namespace model
{
    class Grid;
    enum class TYPE;
}

namespace input
{
    int convertAttrToInt(model::TYPE attr);

    // Parse input file into grid. Returns true on success.
    // Keep string-path overload for CLI convenience; implementations may offer istream variant.
    model::Grid parseInput(const std::string &path);
}
