#!/usr/bin/env bash

cd "${0%/*}"

std="${2:-c++17}"

for cpp in *.cpp; do
    [[ $cpp == _* ]] && continue
    echo "${cpp%.cpp}"

    expected="$(sed -n '1s:^\s*//\s*::p' "$cpp")"
    if [[ "$expected" =~ ^static_assert\ +(.*) ]]; then
        if [ "$1" == 'cl' ]; then
            expected="error C[0-9]\\+: static_assert failed: '${BASH_REMATCH[1]}'"
        else
            expected="error: static assertion failed: ${BASH_REMATCH[1]}"
        fi
    fi

    if [ "$1" == 'cl' ]; then
        cmd=(cl /EHsc /std:"$std" /I../.. /I../../include /Fe/dev/null "$cpp")
    else
        cmd=("${1:-g++}" -std="$std" -I../.. -I../../include "$cpp" -o /dev/null)
    fi

    echo -e '\033[34m'"${cmd[@]}"'\033[0m'
    "${cmd[@]}" 2>&1 | grep -q "$expected"

    declare -a r=("${PIPESTATUS[@]}")
    [ "${r[0]}" -ne 0 ] && [ "${r[1]}" -eq 0 ] \
        && printf '\033[32mPASSED\033[0m\n' \
        || printf '\033[31mFAILED\033[0m\n'
done
