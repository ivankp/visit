#!/usr/bin/env bash

set -e

for cpp in test-*.cpp; do
    exe="${cpp%.cpp}"
    echo "$exe"
    g++ -std=c++17 -Wall -Wextra -pedantic -O3 -I. -I../include "$cpp" -o "$exe"
    ./"$exe"
done
