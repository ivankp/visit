#include "visit.hpp"
#include <any>

template <typename To>
struct VisitADL<std::any, To> {
    template <typename From>
    static bool match(From&& from) {
        return from.type() == typeid(To);
    }

    template <typename From>
    static To convert(From&& from) {
        return std::any_cast<To>(std::forward<From>(from));
    }
};

#define EXAMPLE_STD_ANY
