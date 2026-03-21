#pragma once

#include <string>
#include <fstream>

namespace mylog
{
    extern std::ofstream Logfile;

    bool init_log(const std::string &filename);

    void close_log();

    void output_log(const std::string &message);
}