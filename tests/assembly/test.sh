#!/usr/bin/env bash

cmd=("${1:-g++}" -Wall -Wextra -pedantic -O1 \
  -I../../include \
  -S test.cpp -o -)

echo "${cmd[@]}"
"${cmd[@]}" | sed '/^\s*$/d;/^\s*[.#]/d;s/\s*#.*//'
