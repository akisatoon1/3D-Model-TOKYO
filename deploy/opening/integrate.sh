#!/bin/bash
# integration_deploy.shから呼び出される前提

set -e

input=hantei/integrated/plain.dat
output=opening/integrated/plain.dat
./opening/opening $input $output --size 9

input=hantei/integrated/city.dat
output=opening/integrated/city.dat
./opening/opening $input $output --size 3

cat opening/integrated/plain.dat opening/integrated/city.dat > opening/integrated/all.dat
