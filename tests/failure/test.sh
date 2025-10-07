#!/usr/bin/env bash

cd "${0%/*}"

std="${2:-c++17}"

for src in *.cpp; do
    [[ $src == _* ]] && continue
    echo "${src%.cpp}"

    expected="$(sed -n '1s:^\s*//\s*::p' "$src")"

    if [ "$1" == 'cl' ]; then
        cmd=(cl /EHsc /std:"$std" /I../.. /I../../include /Fe/dev/null "$src")
    else
        cmd=("${1:-g++}" -std="$std" -I../.. -I../../include "$src" -o /dev/null)
    fi

    echo -e '\033[34m'"${cmd[@]}"'\033[0m'
    "${cmd[@]}" 2>&1 | grep -q "$expected"

    declare -a r=("${PIPESTATUS[@]}")
    [ "${r[0]}" -ne 0 ] && [ "${r[1]}" -eq 0 ] \
        && printf '\033[32mPASSED\033[0m\n' \
        || printf '\033[31mFAILED\033[0m\n'
done
