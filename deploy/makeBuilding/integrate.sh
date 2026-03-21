#!/bin/bash
# !!注意!! deploy.shから呼び出すことを想定

set -e

input=opening/integrated/plain.dat
output=makeBuilding/integrated/plain_building.wrl
floor_output=makeBuilding/integrated/plain_floors.dat
log=makeBuilding/integrated/plain.log

./makeBuilding/vrml --input $input --out-vrml $output --out-floor $floor_output --log $log --param palace_param

input=opening/integrated/city.dat
output=makeBuilding/integrated/city_building.wrl
floor_output=makeBuilding/integrated/city_floors.dat
log=makeBuilding/integrated/city.log

./makeBuilding/vrml --input $input --out-vrml $output --out-floor $floor_output --log $log --param param

cat makeBuilding/integrated/plain_floors.dat makeBuilding/integrated/city_floors.dat > makeBuilding/integrated/floors.dat
