#!/bin/bash
# !!注意!! deploy.shから呼び出すことを想定

set -e

num=$1
input=newMap/output/${num}.dat
output=hantei/output/${num}.dat

if [[ $num -eq 53394610 || $num -eq 53394620 ]]; then
    python3 hantei/Ground_detect_plain.py $input $output
else
    python3 hantei/Ground_detect_city.py $input $output
fi
