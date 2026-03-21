#/bin/bash

./clip.sh

dir="newMap"

num=11
origin_num=10
cat mapdata/533946${origin_num}_dsm_1m.dat ${dir}/plain/533946${num}.dat > ${dir}/output/533946${origin_num}.dat

origin_num=11
cp ${dir}/city/533946${origin_num}.dat ${dir}/output/533946${origin_num}.dat

num=20
origin_num=21
cat ${dir}/city/533946${origin_num}.dat ${dir}/city/533946${num}.dat > ${dir}/output/533946${origin_num}.dat
num=21
origin_num=20
cat ${dir}/plain/533946${origin_num}.dat ${dir}/plain/533946${num}.dat > ${dir}/output/533946${origin_num}.dat


nums=(30 31 40 41)
for num in ${nums[@]}; do
    cat ../mapdata/533946${num}_dsm_1m.dat > ${dir}/output/533946${num}.dat
done
