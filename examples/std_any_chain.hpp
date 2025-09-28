#include "visit.hpp"
#include <any>

template <typename To>
struct VisitADL<std::any, To> {
    template <typename From>
    static auto* match(From&& from) {
        return std::any_cast<std::decay_t<To>>(&from);
    }

    template <typename Matched>
    static To convert(Matched&& matched) {
        return static_cast<To>(*matched);
    }
};

#define EXAMPLE_STD_ANY
