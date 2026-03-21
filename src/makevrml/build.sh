#!/bin/bash
g++ -std=c++17 -Wall -I. -I ../module ../module/building.cpp ../module/input.cpp ../module/log.cpp ../module/model.cpp output.cpp algo.cpp params.cpp main.cpp -o vrml `pkg-config --cflags --libs opencv4`
