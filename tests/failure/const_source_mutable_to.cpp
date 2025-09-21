// binding reference of type ‘int&’ to ‘const int’ discards qualifiers

#include "examples/inheritance.hpp"

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](int& x) {
            x = 17;
        }
    );
}
