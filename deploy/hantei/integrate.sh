#!/bin/bash
# integration_deploy.shから呼び出される前提

set -e

nums=(53394610 53394611 53394620 53394621 53394630 53394631 53394640 53394641)
for num in "${nums[@]}"; do
    ./hantei/hantei.sh "$num"
done

mkdir -p hantei/integrated
city=(53394611 53394621 53394630 53394631 53394640 53394641)
cat hantei/output/${city[0]}.dat hantei/output/${city[1]}.dat hantei/output/${city[2]}.dat hantei/output/${city[3]}.dat hantei/output/${city[4]}.dat hantei/output/${city[5]}.dat > hantei/integrated/city.dat

plain=(53394610 53394620)
cat hantei/output/${plain[0]}.dat hantei/output/${plain[1]}.dat > hantei/integrated/plain.dat
