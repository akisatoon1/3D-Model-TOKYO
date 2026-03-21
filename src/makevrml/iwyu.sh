#!/bin/bash
check=include-what-you-use

$check main.cpp
$check io.hpp
$check io.cpp
$check algo.hpp
$check $(pkg-config --cflags opencv4) algo.cpp
# $check model.hpp
# $check model.cpp
