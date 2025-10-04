#include "visit.hpp"

template <>
struct visit::VisitADL<int, int> {
    bool match(int) { return true; }
    int convert(int x) { return x; }
};

int main() {
    int i = 0;
    visit::Visit(5, [&](int x) { i = x; });
    return i;
}
