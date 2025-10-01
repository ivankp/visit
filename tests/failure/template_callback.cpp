// static assertion failed: This callable must have a unique list of arguments. This may fail for templates and overloaded functions.

#include "examples/inheritance.hpp"
using visit::Visit;

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](const auto&) {
        }
    );
}
