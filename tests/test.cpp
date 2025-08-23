#include "test.hpp"

#include <boost/any.hpp>

#include <type_traits>
#include <utility>
#include <vector>

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

TEST(Visit_xxx) {
    Visit(boost::any(1),
        +[](int x, int) {
            TEST_EQ(x, 1);
        }
    );
}

TEST(Visit_Single) {
    bool called = false;
    Visit(boost::any(1),
        [&](int x) {
            called = true;
            TEST_EQ(x, 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByConstRef) {
    bool called = false;
    Visit(boost::any(1),
        [&](const boost::any& x) {
            called = true;
            TEST_EQ(boost::any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByMutRef) {
    bool called = false;
    boost::any x(1);
    Visit(x,
        [&](boost::any& x) {
            called = true;
            TEST_EQ(boost::any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByRRef) {
    bool called = false;
    Visit(boost::any(1),
        [&](boost::any&& x) {
            called = true;
            TEST_EQ(boost::any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByValue) {
    bool called = false;
    Visit(boost::any(1),
        [&](boost::any x) {
            called = true;
            TEST_EQ(boost::any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}

TEST(Visit_Single_MultipleCallbacks) {
    bool called_int = false;
    Visit(boost::any(1),
        [&](int x) {
            called_int = true;
            TEST_EQ(x, 1);
        },
        [](unsigned) {
            TEST_FAIL;
        },
        [](const boost::any&) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called_int);

    called_int = false;
    Visit(boost::any(1),
        [](unsigned) {
            TEST_FAIL;
        },
        [&](int x) {
            called_int = true;
            TEST_EQ(x, 1);
        },
        [](const boost::any&) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called_int);

    bool called_any = false;
    Visit(boost::any(1),
        [](unsigned) {
            TEST_FAIL;
        },
        [&](const boost::any& x) {
            called_any = true;
            TEST_EQ(boost::any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called_any);
}

/*
TEST(Visit_AnyInsideAny) {
    boost::any outer;
    boost::any inner(2);
    outer = inner;
    cout << inner.type().name() << endl;
    cout << outer.type().name() << endl;
    TEST_EQ(boost::any_cast<int>(inner), 2);
    TEST_EQ(boost::any_cast<int>(outer), 2);

    bool called = false;
    Visit(outer,
        [&](const boost::any& x) {
            called = true;
            TEST_EQ(boost::any_cast<int>(x), 2);
        }
    );
    TEST_TRUE(called);
}
*/

#if __cplusplus < 202002L
template <typename Xs, typename... F>
auto VisitEach(Xs&& xs, F&&... callback)
-> decltype((std::begin(xs) != std::end(xs)), void{})
{
    for (auto&& x : xs) {
        Visit(x, callback...);
    }
}
#endif

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
