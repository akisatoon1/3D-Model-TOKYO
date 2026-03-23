#!/bin/bash

set -e

echo "Integration Version"

echo "hantei"
./hantei/integrate.sh

echo "opening"
./opening/integrate.sh

echo "makeBuilding"
./makeBuilding/integrate.sh

echo "makeGround"
./makeGround/integrate.sh

mkdir -p wrl
echo "#VRML V2.0 utf8

Inline {
    url \"../makeBuilding/integrated/city_building.wrl\"
}

Inline {
    url \"../makeBuilding/integrated/plain_building.wrl\"
}

Inline {
    url \"../makeGround/integrated/floors.wrl\"
}
" > wrl/integrated.wrl

echo "Integration completed. Output: wrl/integrated.wrl"
