#include "test.hpp"

#include <boost/any.hpp>

#include <vector>
#include <utility>
#include <type_traits>

#if __cplusplus >= 202002L
#include "visit-cpp20.hpp"
#else
#include "visit-cpp14.hpp"
#endif

using std::cout;
using std::endl;

template <typename To>
struct VisitADL<boost::any, To> {
    template <typename X>
    static bool match(X&& x) noexcept {
        return x.type() == typeid(To);
    }

    template <typename X>
    static To convert(X&& x) noexcept {
        return boost::any_cast<To>(std::forward<X>(x));
    }
};

TEST(Visit_SingleValue) {
    bool called = false;
    Visit(boost::any(1),
        [&](int x) {
            called = true;
            TEST_EQ(x, 1);
        },
        [](const boost::any&) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);

    called = false;
    Visit(boost::any(1),
        [&](boost::any x) {
            called = true;
            TEST_EQ(boost::any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}

template <typename Xs, typename... F>
auto VisitEach(Xs&& xs, F&&... callback)
-> decltype((std::begin(xs) != std::end(xs)), void{})
{
    for (auto&& x : xs) {
        Visit(x, callback...);
    }
}

TEST(visit_vector) {
    std::vector<boost::any> xs { 3, 2.5, 'D', 71, "Text" };

    int i = 0;
    VisitEach(xs,
        [&i](int x) {
            switch (i++) {
                case 0:
                    TEST_EQ(x, 3);
                    break;
                case 3:
                    TEST_EQ(x, 71);
                    break;
                default:
                    TEST_FAIL;
            }
        },
        [&i](char x) {
            switch (i++) {
                case 2:
                    TEST_EQ(x, 'D');
                    break;
                default:
                    TEST_FAIL;
            }
        },
        [&i](const char* x) {
            switch (i++) {
                case 4:
                    TEST_FALSE(strcmp(x, "Text"));
                    break;
                default:
                    TEST_FAIL;
            }
        },
        [&i](double x) {
            switch (i++) {
                case 1:
                    TEST_EQ(x, 2.5);
                    break;
                default:
                    TEST_FAIL;
            }
        },
        [&i](const boost::any&) {
            TEST_FAIL;
        }
    );
}
