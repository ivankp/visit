#include "visit.hpp"

#include <boost/any.hpp>

template <typename To>
struct VisitADL<boost::any, To> {
    template <typename Visited>
    static bool match(Visited&& visited) {
        return visited.type() == typeid(To);
    }

    template <typename Visited>
    static To convert(Visited&& visited) {
        return boost::any_cast<To>(std::forward<Visited>(visited));
    }
};
