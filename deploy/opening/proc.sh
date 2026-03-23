#!/bin/bash
# !!注意!! deploy.shから呼び出すことを想定

set -e

num=$1
echo "Processing number: $num"
input=hantei/output/${num}.dat
output=opening/output/${num}.dat
mkdir -p opening/output

if [[ $num -eq 53394610 || $num -eq 53394620 ]]; then
    ./opening/opening $input $output --size 9
else
    ./opening/opening $input $output --size 3
fi
