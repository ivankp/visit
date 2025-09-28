#!/usr/bin/env bash

set -e

cd "${0%/*}"
mkdir -p build

stds=(c++17 c++20 c++23)
compilers=(g++ clang++)

srcs=(tests.cpp test.hpp ../include/{callable,visit}.hpp)
exes=()
pids=()

for example in ../examples/*.hpp; do
    name="${example##*/}"
    name="${name%%.hpp}"

    newest="test.sh"
    for src in "${srcs[@]}" "$example"; do
        if [ "$src" -nt "$newest" ]; then
            newest="$src"
        fi
    done

    for comp in "${compilers[@]}"; do
    for std in "${stds[@]}"; do
        exe="build/$name-$std-$comp"
        exes+=("$exe")
        if [ ! -f "$exe" ] || [ "$newest" -nt "$exe" ]; then
            echo "Compiling $exe"
            "$comp" -std="$std" -Wall -Wextra -Werror -pedantic -O3 \
                -DEXAMPLE="$name" -I. -I../include \
                "${srcs[0]}" -o "$exe" &
            pids+=($!)
        fi
    done
    done
done

for pid in ${pids[@]}; do
    wait $pid || exit 1
done

for exe in "${exes[@]}"; do
    echo "$exe"
    ./"$exe"
done

./failure/test.sh
