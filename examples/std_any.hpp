#include "visit.hpp"
#include <any>

namespace visit {

template <typename To>
struct VisitADL<std::any, To> {
    template <typename From>
    static bool match(From&& from) noexcept {
        return from.type() == typeid(To);
    }

    template <typename From>
    static To convert(From&& from) noexcept {
        return std::any_cast<To>(std::forward<From>(from));
    }
};

}

#define EXAMPLE_STD_ANY
