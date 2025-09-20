#!/usr/bin/env bash

declare -A error=(
    [ADL_not_defined]='static assertion failed: VisitADL is not specialized for these types'
    [no_args_callback]='static assertion failed: Unexpected number of callback arguments.'
    [mutable_to_const_from]='binding reference of type ‘int&’ to ‘const int’ discards qualifiers'
)

for cpp in *.cpp; do
    [[ $cpp == _* ]] && continue
    exe="${cpp%.cpp}"
    echo "$exe"
    g++ -std=c++17 -I.. -I../../include "$cpp" -o /dev/null 2>&1 \
    | grep -Fq "error: ${error["$exe"]}"
    [ "${PIPESTATUS[*]}" == '1 0' ] && echo 'PASSED' || echo 'FAILED'
done
