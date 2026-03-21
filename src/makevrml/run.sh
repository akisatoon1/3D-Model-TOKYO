#!/bin/bash

./build.sh

# nums=(53394610 53394611 53394620 53394621 53394630 53394631 53394640 53394641)

# for number in "${nums[@]}"; do

number=53394611
input=../../deploy/opening/output/${number}.dat
output=../../vrml/wrl/${number}.wrl
floor_output=../../vrml/wrl/floor/${number}_floors.dat
log=../../vrml/wrl/log/${number}.log

./vrml --input $input --out-vrml $output --out-floor $floor_output --log $log --param param

# done
