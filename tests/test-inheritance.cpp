#define USE_PRINTF
#define VISIT

#ifdef VISIT
#include "visit-cpp20.hpp"
#include <type_traits>
#endif

#if defined(USE_IOSTREAM)
#include <iostream>

#elif defined(USE_PRINTF)
#include <cstdio>

#elif defined(USE_FMT)
#include <fmt/base.h>

#endif

#define STR1(X) #X
#define STR(X) STR1(X)

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
    using To_t = std::remove_cvref_t<To>;

    template <typename X>
    static bool match(X&& x) noexcept {
        return dynamic_cast<Derived<To_t>*>(&x);
    }

    template <typename X>
    static To convert(X&& x) noexcept {
        return dynamic_cast<Derived<To_t>*>(&x)->value;
    }
};
#endif

int main() {
    Base* a = new Derived<int>(5);
    Base* b = new Derived<const char*>("text");
    Base* c = new Base;

#ifdef VISIT
    Visit(*a,
#if defined(USE_IOSTREAM)
        [](int x){ std::cout << x << std::endl; },
        [](const char* x){ std::cout << x << std::endl; },
        [](const Base& x){ std::cout << &x << std::endl; }
#elif defined(USE_PRINTF)
        [](int x){ printf("%d\n", x); },
        [](const char* x){ printf("%s\n", x); },
        [](const Base& x){ printf("%p\n", &x); }
#elif defined(USE_FMT)
        [](int x){ fmt::print("{}\n", x); },
        [](const char* x){ fmt::print("{}\n", x); },
        [](const Base& x){ fmt::print("{}\n", (void*)&x); }
#endif
    );
    Visit(*b,
#if defined(USE_IOSTREAM)
        [](int x){ std::cout << x << std::endl; },
        [](const char* x){ std::cout << x << std::endl; },
        [](const Base& x){ std::cout << &x << std::endl; }
#elif defined(USE_PRINTF)
        [](int x){ printf("%d\n", x); },
        [](const char* x){ printf("%s\n", x); },
        [](const Base& x){ printf("%p\n", &x); }
#elif defined(USE_FMT)
        [](int x){ fmt::print("{}\n", x); },
        [](const char* x){ fmt::print("{}\n", x); },
        [](const Base& x){ fmt::print("{}\n", (void*)&x); }
#endif
    );
    Visit(*c,
#if defined(USE_IOSTREAM)
        [](int x){ std::cout << x << std::endl; },
        [](const char* x){ std::cout << x << std::endl; },
        [](const Base& x){ std::cout << &x << std::endl; }
#elif defined(USE_PRINTF)
        [](int x){ printf("%d\n", x); },
        [](const char* x){ printf("%s\n", x); },
        [](const Base& x){ printf("%p\n", &x); }
#elif defined(USE_FMT)
        [](int x){ fmt::print("{}\n", x); },
        [](const char* x){ fmt::print("{}\n", x); },
        [](const Base& x){ fmt::print("{}\n", (void*)&x); }
#endif
    );

#else
#if defined(USE_IOSTREAM)
    std::cout << STR(__LINE__) " " << dynamic_cast<Derived<int>*>(a)->value << std::endl;
    std::cout << STR(__LINE__) " " << dynamic_cast<Derived<const char*>*>(b)->value << std::endl;
    std::cout << STR(__LINE__) " " << c << std::endl;
#elif defined(USE_PRINTF)
    printf(STR(__LINE__) " %d\n", dynamic_cast<Derived<int>*>(a)->value);
    printf(STR(__LINE__) " %s\n", dynamic_cast<Derived<const char*>*>(b)->value);
    printf(STR(__LINE__) " %p\n", c);
#elif defined(USE_FMT)
    fmt::print(STR(__LINE__) " {}\n", dynamic_cast<Derived<int>*>(a)->value);
    fmt::print(STR(__LINE__) " {}\n", dynamic_cast<Derived<const char*>*>(b)->value);
    fmt::print(STR(__LINE__) " {}\n", (void*)c);
#endif


#endif
}
