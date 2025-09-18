// #define VISIT

#ifdef VISIT
#include "visit-cpp20.hpp"
#endif

#include <type_traits>

/*
struct Foo {
    enum Tag { NONE, INT, DOUBLE } tag;
    union { int i; double d; };

    template <typename T>
    static constexpr Tag tag_v = NONE;

    template <>
    static constexpr Tag tag_v<int> = INT;

    template <>
    static constexpr Tag tag_v<double> = DOUBLE;

    Foo(): tag(NONE) { }
    Foo(int i): tag(INT), i(i) { }
    Foo(double d): tag(DOUBLE), d(d) { }
};

template <typename To>
struct VisitADL<Foo, To, std::enable_if_t<>> {
    template <typename X>
    static bool match(X&& x) noexcept {
        return x.tag == Foo::tag_v<To>;
    }

    template <typename X>
    static To convert(X&& x) noexcept {
        if (x.tag == INT)
    }
};

int main() {
    Wrap a(1), b(2.), c;

}
*/

struct Base {
    virtual ~Base() = default;
};

template <typename T>
struct Derived : Base {
    T value;
    Derived(T x): value(x) { }
};

#ifdef VISIT
template <typename To>
struct VisitADL<Base, To> {
    using Type = std::remove_cvref_t<To>;

    template <typename X>
    static bool match(X&& x) noexcept {
        return dynamic_cast<Derived<Type>*>(&x);
    }

    template <typename X>
    static To convert(X&& x) noexcept {
        return dynamic_cast<Derived<Type>*>(&x)->value;
    }
};
#endif

int main() {
    Base* a = new Derived<int>(1);
    Base* b = new Derived<double>(2.);
    Base* c = new Base;

#ifdef VISIT
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

#else
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
#endif
}
