// Include all the standard library headers included by visit.hpp
#include <type_traits>
#include <utility>

struct Base {
    virtual ~Base() = default;
};

template <typename T>
struct Derived : Base {
    T value;
    Derived(T x): value(x) { }
};

int main() {
    Base* a = new Derived<int>(1);
    Base* b = new Derived<double>(2.);
    Base* c = new Base;

    int result = 0;

    if (auto* x = dynamic_cast<Derived<int>*>(a)) {
        result = x->value == 1 ? 1 : -1;
    } else if (auto* x = dynamic_cast<Derived<double>*>(a)) {
        result = x->value == 2. ? 2 : -2;
    } else {
        result = 4;
    }

    if (result != 1)
        return result;

    result = 0;

    if (auto* x = dynamic_cast<Derived<int>*>(b)) {
        result = x->value == 1 ? 1 : -1;
    } else if (auto* x = dynamic_cast<Derived<double>*>(b)) {
        result = x->value == 2. ? 2 : -2;
    } else {
        result = 4;
    }

    if (result != 2)
        return result;

    result = 0;

    if (auto* x = dynamic_cast<Derived<int>*>(c)) {
        result = x->value == 1 ? 1 : -1;
    } else if (auto* x = dynamic_cast<Derived<double>*>(c)) {
        result = x->value == 2. ? 2 : -2;
    } else {
        result = 4;
    }

    if (result != 4)
        return result;
}
