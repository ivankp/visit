#include "visit-inheritance.hpp"

int main() {
    const Base& x = Derived<int>(113);

    Visit(x,
        [&](int& x) {
            x = 17;
        }
    );
}
