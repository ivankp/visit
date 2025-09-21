// binding reference of type ‘Base&’ to ‘const Base’ discards qualifiers

#include "examples/inheritance.hpp"

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](Base& x) {
            static_cast<Derived<int>&>(x).value = 1;
        }
    );
}
