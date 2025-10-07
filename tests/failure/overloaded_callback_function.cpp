// overloaded

#include "examples/inheritance.hpp"
using visit::Visit;

void visitor(int) { }
void visitor(float) { }

int main() {
    const Base& x = Derived<int>(113);

    Visit(x, visitor);
}
