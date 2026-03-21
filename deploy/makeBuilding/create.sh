#!/bin/bash
# !!注意!! deploy.shから呼び出すことを想定

set -e

num=$1
input=opening/output/${num}.dat
output=makeBuilding/wrl/${num}_building.wrl
floor_output=makeBuilding/floor/${num}_floors.dat
log=makeBuilding/log/${num}.log

if [[ $num -eq 53394610 || $num -eq 53394620 ]]; then
    ./makeBuilding/vrml --input $input --out-vrml $output --out-floor $floor_output --log $log --param palace_param
else
    ./makeBuilding/vrml --input $input --out-vrml $output --out-floor $floor_output --log $log --param param
fi
