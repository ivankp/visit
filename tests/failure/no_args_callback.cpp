// static assertion failed: Visit callbacks must have either 1 or 2 arguments.

#include "examples/inheritance.hpp"
using visit::Visit;

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&]() {
        }
    );
}
