#!/bin/bash

xmin=0
ymin=0
xmax=100
ymax=100

input=input_data.txt
clipped=clipped.dat
residual=residual.dat

python3 clip.py ${xmin} ${ymin} ${xmax} ${ymax} ${input} ${clipped} ${residual}
