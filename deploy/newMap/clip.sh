#/bin/bash

dir="newMap"

num=11
python3 clip.py -7000 -35497 -6348 -30000 mapdata/533946${num}_dsm_1m.dat ${dir}/tmp/clip_1.dat ${dir}/tmp/origin_1.dat
python3 clip.py -6347 -35227 -6267 -30000 ${dir}/tmp/origin_1.dat ${dir}/tmp/clip_2.dat ${dir}/tmp/origin_2.dat
cat ${dir}/tmp/clip*.dat > ${dir}/plain/533946${num}.dat
cp ${dir}/tmp/origin_2.dat ${dir}/city/533946${num}.dat
rm ${dir}/tmp/*

num=20
python3 clip.py -7199 -34291 -6000 -30000 mapdata/533946${num}_dsm_1m.dat ${dir}/tmp/clip_1.dat ${dir}/tmp/origin_1.dat
python3 clip.py -7150 -34366 -6000 -34292 ${dir}/tmp/origin_1.dat ${dir}/tmp/clip_2.dat ${dir}/tmp/origin_2.dat
python3 clip.py -6824 -34434 -6000 -34367 ${dir}/tmp/origin_2.dat ${dir}/tmp/clip_3.dat ${dir}/tmp/origin_3.dat
python3 clip.py -6736 -34672 -6000 -34435 ${dir}/tmp/origin_3.dat ${dir}/tmp/clip_4.dat ${dir}/tmp/origin_4.dat
cat ${dir}/tmp/clip_*.dat > ${dir}/city/533946${num}.dat
cp ${dir}/tmp/origin_4.dat ${dir}/plain/533946${num}.dat
rm ${dir}/tmp/*

num=21
python3 clip.py -7000 -36000 -6265 -34797 mapdata/533946${num}_dsm_1m.dat ${dir}/tmp/clip_1.dat ${dir}/tmp/origin_1.dat
python3 clip.py -7000 -34796 -6321 -34763 ${dir}/tmp/origin_1.dat ${dir}/tmp/clip_2.dat ${dir}/tmp/origin_2.dat
python3 clip.py -7000 -34762 -6367 -34726 ${dir}/tmp/origin_2.dat ${dir}/tmp/clip_3.dat ${dir}/tmp/origin_3.dat
cat ${dir}/tmp/clip_*.dat > ${dir}/plain/533946${num}.dat
cp ${dir}/tmp/origin_3.dat ${dir}/city/533946${num}.dat
rm ${dir}/tmp/*
