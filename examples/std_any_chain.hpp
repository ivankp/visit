#include "visit.hpp"
#include <any>

template <typename To>
struct VisitADL<std::any, To> {
    template <typename From>
    static auto* match(From&& from) noexcept {
        return std::any_cast<std::decay_t<To>>(&from);
    }

    template <typename Matched>
    static To convert(Matched* matched) noexcept {
        return static_cast<To>(*matched);
    }
};

#define EXAMPLE_STD_ANY
