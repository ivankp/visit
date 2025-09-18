#!/usr/bin/env bash

if ! [[ $1 =~ [0-9]+ ]]; then
    echo "usage: $0 [number of runs]" >&2
    exit 1
fi

set -e
TIMEFORMAT='%S %U'

time_compilation() {
    time \
        g++ -std=c++17 -Wall -O3 -I../../include "$@" -o /dev/null
}

N="$1"
for (( i = 0; i < N; ++i )); do
    time_compilation with-visit.cpp 2>&1
    time_compilation without-visit.cpp 2>&1
done | python -c '''
import sys, math
ts = [ sum(float(t) for t in line.split()) for line in sys.stdin ]
def mean_stdev(xs):
    n = len(xs)
    m = sum(xs)/n
    s = math.sqrt(sum( (x - m)**2 for x in xs ) / (n - 1))
    return (m, s)
t = [ mean_stdev(ts[i::2]) for i in (0,1) ]
d = min(math.ceil(-math.log10(min(t[1] for t in t))) + 1, 6)
dt = (t[0][0] - t[1][0], math.sqrt(t[0][1]**2 + t[1][1]**2))
def p(m,s):
    return f"{m:.{d}f} ± {s:.{d}f}"
print(f"""\
With    visit: { p(*t[0]) }
Without visit: { p(*t[1]) }
Difference   : { p(*dt) }\
""")
'''
