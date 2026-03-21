#!/bin/bash

set -e

# スクリプトのディレクトリを取得
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# カレントディレクトリを取得
CURRENT_DIR="$(pwd)"

# ディレクトリが一致するかチェック
if [ "$SCRIPT_DIR" != "$CURRENT_DIR" ]; then
    echo "Error: This script must be run from its own directory."
    echo "Script directory: $SCRIPT_DIR"
    echo "Current directory: $CURRENT_DIR"
    exit 1
fi

# 引数のチェック
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <マップ番号の末尾2桁(ex: 10, 11)>"
    exit 1
fi

num=533946$1
echo "Processing number: $num"
echo "-------------------------"

<< COMMENTOUT
echo "Step 0: Clip map"
./newMap/run.sh
COMMENTOUT

echo "-------------------------"
echo "Step 1: Hantei"
./hantei/hantei.sh $num

echo "-------------------------"
echo "Step 2: Opening"
./opening/proc.sh $num

echo "-------------------------"
echo "Step 3: Make Building"
./makeBuilding/create.sh $num

echo "-------------------------"
echo "Step 4: Make Ground"
./makeGround/create.sh $num

echo "-------------------------"

echo "#VRML V2.0 utf8

Inline {
    url \"../makeBuilding/wrl/${num}_building.wrl\"
}

Inline {
    url \"../makeGround/wrl/${num}_ground.wrl\"
}
" > wrl/${num}.wrl

echo "Deployment completed."
echo "See ${num}.wrl to confirm the result."
