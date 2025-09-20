#include "visit.hpp"

#ifdef TEST_BOOST_ANY
#include <boost/any.hpp>
using boost::any, boost::any_cast;
#endif

#ifdef TEST_STD_ANY
#include <any>
using std::any, std::any_cast;
#endif

template <typename To>
struct VisitADL<any, To> {
    template <typename Visited>
    static bool match(Visited&& visited) {
        return visited.type() == typeid(To);
    }

    template <typename Visited>
    static To convert(Visited&& visited) {
        return any_cast<To>(std::forward<Visited>(visited));
    }
};
