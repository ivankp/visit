#include "test.hpp"

#include <boost/any.hpp>

#include <vector>
#include <utility>
#include <type_traits>

using std::cout;
using std::endl;

#if __cplusplus >= 202002L
template <typename X, typename T>
concept DecaysTo = std::same_as<T, std::decay_t<X>>;

template <typename T, DecaysTo<boost::any> X>
bool match(X&& x) noexcept {
    return x.type() == typeid(T);
}

template <typename T, DecaysTo<boost::any> X>
T convert(X&& x) {
    return boost::any_cast<T>(std::forward<X>(x));
}

#else

template <typename X, typename T>
constexpr bool decaysTo = std::is_same<T, std::decay_t<X>>::value;

template <typename T, typename X>
auto match(X&& x) noexcept -> std::enable_if_t<decaysTo<X, boost::any>, bool> {
    return x.type() == typeid(T);
}

template <typename T, typename X>
auto convert(X&& x) noexcept -> std::enable_if_t<decaysTo<X, boost::any>, T> {
    return boost::any_cast<T>(std::forward<X>(x));
}
#endif

#if __cplusplus >= 202002L
#include "visit-cpp20.hpp"
#else
#include "visit-cpp14.hpp"
#endif

TEST(Visit_match) {
    boost::any x(1);
    TEST_TRUE(match<int>(x));
    TEST_FALSE(match<unsigned>(x));
}

TEST(Visit_convert) {
    TEST_EQ(convert<int>(boost::any(5)), 5);
    TEST_EQ(convert<double>(boost::any(2.3)), 2.3);
}

TEST(Visit_SingleValue) {
    boost::any x(1);

    Visit(x,
        [](int x) {
            TEST_EQ(x, 1);
        },
        [](const boost::any&) {
            TEST_FAIL;
        }
    );
}

template <typename Xs, typename... F>
auto VisitEach(Xs&& xs, F&&... callback)
-> decltype((std::begin(xs) != std::end(xs)), void{})
{
    for (auto&& x : xs) {
        Visit(x, callback...);
    }
}

TEST(visit_any) {
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
