#include "visit.hpp"
#include <boost/any.hpp>

template <typename To>
struct VisitADL<boost::any, To> {
    template <typename From>
    static bool match(From&& from) noexcept {
        return from.type() == typeid(To);
    }

    template <typename From>
    static To convert(From&& from) noexcept {
        return boost::any_cast<To>(std::forward<From>(from));
    }
};

#define EXAMPLE_BOOST_ANY
