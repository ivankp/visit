// static_cast

#include "examples/inheritance.hpp"
using visit::Visit;

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](int& x) {
            x = 17;
        }
    );
}
