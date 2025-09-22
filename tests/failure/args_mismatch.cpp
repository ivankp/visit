// static assertion failed: For a 2-argument callback, one of the arguments must match the visited type.

#include "examples/inheritance.hpp"

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](int, float) {
        }
    );
}
