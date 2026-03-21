#!/bin/bash

nums=(53394610 53394611 53394620 53394621 53394630 53394631 53394640 53394641)

for num in "${nums[@]}"; do
input=../../src/opening/output/${num}.dat
python3 display.py $input ./open_${num}.png
done
