#include "visit.hpp"

struct Base {
    virtual ~Base() = default;
};

template <typename T>
struct Derived : Base {
    T value;
    Derived(T x): value(x) { }
};

template <typename To>
struct VisitADL<Base, To> {
    using Type = std::decay_t<To>;

    template <typename X>
    static bool match(X&& x) noexcept {
        return dynamic_cast<Derived<Type>*>(&x);
    }

    template <typename X>
    static To convert(X&& x) noexcept {
        return dynamic_cast<Derived<Type>*>(&x)->value;
    }
};

int main() {
    Base* a = new Derived<int>(1);
    Base* b = new Derived<double>(2.);
    Base* c = new Base;

    int result = 0;

    Visit(*a,
        [&result](int x){ result = x == 1 ? 1 : -1; },
        [&result](double x){ result = x == 2. ? 2 : -2; },
        [&result](const Base&){ result = 4; }
    );

    if (result != 1)
        return result;

    result = 0;

    Visit(*b,
        [&result](int x){ result = x == 1 ? 1 : -1; },
        [&result](double x){ result = x == 2. ? 2 : -2; },
        [&result](const Base&){ result = 4; }
    );

    if (result != 2)
        return result;

    result = 0;

    Visit(*c,
        [&result](int x){ result = x == 1 ? 1 : -1; },
        [&result](double x){ result = x == 2. ? 2 : -2; },
        [&result](const Base&){ result = 4; }
    );

    if (result != 4)
        return result;
}
