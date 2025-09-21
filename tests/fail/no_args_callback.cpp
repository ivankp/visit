// static assertion failed: Unexpected number of callback arguments.

#include "examples/inheritance.hpp"

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&]() {
        }
    );
}
