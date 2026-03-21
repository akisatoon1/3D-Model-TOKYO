#!/bin/bash

# スクリプトのディレクトリを取得
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

g++ -I "${SCRIPT_DIR}/../module" \
    -o "${SCRIPT_DIR}/opening" \
    "${SCRIPT_DIR}/main.cpp" \
    "${SCRIPT_DIR}/../module/model.cpp" \
    "${SCRIPT_DIR}/../module/input.cpp" \
    "${SCRIPT_DIR}/../module/log.cpp" \
    -std=c++17
