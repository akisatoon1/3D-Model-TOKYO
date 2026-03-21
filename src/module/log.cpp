#include "log.hpp"

#include <fstream>
#include <string>

namespace mylog
{
    std::ofstream Logfile;

    bool init_log(const std::string &filename)
    {
        Logfile.open(filename, std::ios::out);
        return Logfile.is_open();
    }

    void close_log()
    {
        if (Logfile.is_open())
        {
            Logfile.close();
        }
    }

    void output_log(const std::string &message)
    {
        if (Logfile.is_open())
        {
            Logfile << message;
        }
    }
}
