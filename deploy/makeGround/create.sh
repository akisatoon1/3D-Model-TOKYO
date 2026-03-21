#!/bin/bash
# !!注意!! deploy.shから呼び出すことを想定

set -e

num=$1
input=opening/output/${num}.dat
floor=makeBuilding/floor/${num}_floors.dat
output=makeGround/wrl/${num}_ground.wrl

python3 makeGround/delaunay04.py $input $floor $output
