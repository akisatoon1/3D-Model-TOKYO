#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

bool check_arg_num(int n)
{
    return n == 8;
}

string filename(int argc, char *argv[])
{
    return argv[1];
}

void param_min(int argc, char *argv[], int &x, int &y)
{
    x = stoi(argv[2]);
    y = stoi(argv[3]);
}

void param_max(int argc, char *argv[], int &x, int &y)
{
    x = stoi(argv[4]);
    y = stoi(argv[5]);
}

void param_div(int argc, char *argv[], int &x, int &y)
{
    x = stoi(argv[6]);
    y = stoi(argv[7]);
}

int main(int argc, char *argv[])
{
    if (!check_arg_num(argc))
    {
        cerr << "Invalid args number. Expected 8" << endl;
        return 1;
    }

    int minx, miny;
    int maxx, maxy;
    int divx, divy;
    param_min(argc, argv, minx, miny);
    param_max(argc, argv, maxx, maxy);
    param_div(argc, argv, divx, divy);

    vector<string> splitted;
    splitted.resize(divx * divy);

    string fname = filename(argc, argv);
    ifstream f(fname);
    if (!f)
    {
        cerr << "Error opening file" << endl;
        return 1;
    }

    string line;
    while (getline(f, line))
    {
        stringstream ss(line);
        double dx, dy;
        if (!(ss >> dx >> dy))
        {
            cerr << "format error: " << line << endl;
            continue;
        }

        int x = static_cast<int>(dx);
        int y = static_cast<int>(dy);
        if (x < minx || x > maxx || y < miny || y > maxy)
        {
            cerr << "out of range: x:" << x << " y:" << y << endl;
            continue;
        }

        int x_dur = (maxx - minx);
        int y_dur = (maxy - miny);
        int ix = (x - minx) * divx / x_dur;
        int iy = (y - miny) * divy / y_dur;
        if (ix >= divx)
            ix = divx - 1;
        if (iy >= divy)
            iy = divy - 1;

        splitted[ix * divy + iy] += line + "\n";
    }

    for (int i = 0; i < divx; ++i)
    {
        for (int j = 0; j < divy; ++j)
        {
            string outfile = "splitted_xyID_" + to_string(i) + "_" + to_string(j) + ".dat";
            ofstream out(outfile);
            if (!out)
            {
                cerr << "Error creating file: " << outfile << endl;
                continue;
            }
            out << splitted[i * divy + j];
            out.close();
        }
    }

    return 0;
}