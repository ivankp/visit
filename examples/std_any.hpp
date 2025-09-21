#include "visit.hpp"

#include <any>

template <typename To>
struct VisitADL<std::any, To> {
    template <typename Visited>
    static bool match(Visited&& visited) {
        return visited.type() == typeid(To);
    }

    template <typename Visited>
    static To convert(Visited&& visited) {
        return std::any_cast<To>(std::forward<Visited>(visited));
    }
};
