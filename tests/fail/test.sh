#!/usr/bin/env bash

for cpp in *.cpp; do
    [[ $cpp == _* ]] && continue
    echo "${cpp%.cpp}"
    g++ -std=c++17 -I../.. -I../../include "$cpp" -o /dev/null 2>&1 \
    | grep -Fq "error: $(sed -n '1s:^\s*//\s*::p' "$cpp")"
    [ "${PIPESTATUS[*]}" == '1 0' ] && echo 'PASSED' || echo 'FAILED'
done
