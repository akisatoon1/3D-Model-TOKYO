#!/bin/bash
# !!注意!! integrate.shから呼び出すことを想定

set -e

input=opening/integrated/all.dat
floor=makeBuilding/integrated/floors.dat
output=makeGround/integrated/floors.wrl

python3 makeGround/delaunay04.py $input $floor $output
