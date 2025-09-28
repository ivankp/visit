#!/usr/bin/env bash

echo "${0%/*}"

for cpp in *.cpp; do
    [[ $cpp == _* ]] && continue
    echo "${cpp%.cpp}"
    g++ -std=c++17 -I../.. -I../../include "$cpp" -o /dev/null 2>&1 \
    | grep -q "error: $(sed -n '1s:^\s*//\s*::p' "$cpp")"
    declare -a r=("${PIPESTATUS[@]}")
    [ "${r[0]}" -ne 0 ] && [ "${r[1]}" -eq 0 ] \
        && printf '\033[32mPASSED\033[0m\n' \
        || printf '\033[31mFAILED\033[0m\n'
done
