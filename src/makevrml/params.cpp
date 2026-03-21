#include "params.hpp"
#include "log.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace params
{
    BuildingParams loadParams(const std::string &filepath)
    {
        BuildingParams params;
        std::ifstream ifs(filepath);

        if (!ifs.is_open())
        {
            throw std::runtime_error("Failed to open parameter file: " + filepath);
        }

        std::string line;
        int line_number = 0;

        while (std::getline(ifs, line))
        {
            ++line_number;

            // コメント行と空行をスキップ
            if (line.empty() || line[0] == ';' || line[0] == '#')
            {
                continue;
            }

            // KEY=VALUE形式をパース
            size_t eq_pos = line.find('=');
            if (eq_pos == std::string::npos)
            {
                mylog::output_log("Warning: Invalid line format at line " +
                                  std::to_string(line_number) + ": " + line + "\n");
                continue;
            }

            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(eq_pos + 1);

            try
            {
                if (key == "MIN_BUILDING_AREA")
                {
                    params.min_building_area = std::stod(value);
                }
                else if (key == "MAX_ASPECT_RATIO")
                {
                    params.max_aspect_ratio = std::stod(value);
                }
                else if (key == "MIN_CONTOUR_POINTS")
                {
                    params.min_contour_points = std::stoull(value);
                }
                else if (key == "MAX_MEMBERS")
                {
                    params.max_members = std::stoull(value);
                }
                else if (key == "MIN_MEMBERS")
                {
                    params.min_members = std::stoull(value);
                }
                else if (key == "MIN_FILL_RATE")
                {
                    params.min_fill_rate = std::stoull(value);
                }
                else
                {
                    mylog::output_log("Warning: Unknown parameter at line " +
                                      std::to_string(line_number) + ": " + key + "\n");
                }
            }
            catch (const std::exception &e)
            {
                mylog::output_log("Warning: Failed to parse value at line " +
                                  std::to_string(line_number) + ": " + line + "\n");
            }
        }

        return params;
    }
}