#!/usr/bin/env bash

set -e

cd "${0%/*}"
mkdir -p build

IFS=',' read -ra compilers <<< "$1"
[ ${#compilers[@]} -eq 0 ] && compilers=(g++ clang++)

IFS=',' read -ra stds <<< "$2"
[ ${#stds[@]} -eq 0 ] && stds=(c++17 c++20 c++23)

IFS=',' read -ra examples <<< "$3"
[ ${#examples[@]} -eq 0 ] && \
for example in ../examples/*.hpp; do
    example="${example##*/}"
    examples+=("${example%%.hpp}")
done

srcs=(tests.cpp test.hpp ../include/{callable,visit}.hpp)
exes=()
pids=()

for example in "${examples[@]}"; do
    newest="test.sh"
    for src in "${srcs[@]}" "../examples/${example}.hpp"; do
        if [ "$src" -nt "$newest" ]; then
            newest="$src"
        fi
    done

    for comp in "${compilers[@]}"; do
    for std in "${stds[@]}"; do
        exe="build/$example-$std-$comp"
        exes+=("$exe")
        if [ ! -f "$exe" ] || [ "$newest" -nt "$exe" ]; then
            if [ "$comp" == 'cl' ]; then
                cmd=("$comp" /EHsc /std:"$std" /O2 /W3 /WX \
                    /DEXAMPLE="$example" /I. /I.. /I../include \
                    /Fe"$exe" "${srcs[0]}")
            else
                cmd=("$comp" -std="$std" -O3 -Wall -Wextra -Werror -pedantic \
                    -DEXAMPLE="$example" -I. -I.. -I../include \
                    "${srcs[0]}" -o "$exe")
            fi
            echo -e '\033[34m'"${cmd[@]}"'\033[0m'
            if [ "$comp" == 'cl' ]; then
                "${cmd[@]}"
            else
                "${cmd[@]}" &
            fi
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

[ $# -eq 0 ] && ./failure/test.sh
