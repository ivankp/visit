#include "visit.hpp"

struct S { int i; };

int main() {
    S s;
    Visit(s, [&](int s) { });
}
