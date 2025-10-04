// const Base

#include "examples/inheritance.hpp"
using visit::Visit;

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](Base& x) {
            static_cast<Derived<int>&>(x).value = 1;
        }
    );
}
