// static_assert Visit() must be called with at least 1 callback argument.

#include "examples/inheritance.hpp"
using visit::Visit;

int main() {
    const Base& x = Derived<int>(113);

    Visit(x);
}
