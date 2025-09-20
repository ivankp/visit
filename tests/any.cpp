#include "test.hpp"

#include "visit.hpp"

#ifdef BOOST_ANY
#include <boost/any.hpp>
using boost::any, boost::any_cast;
#endif

#ifdef STD_ANY
#include <any>
using std::any, std::any_cast;
#endif

template <typename To>
struct VisitADL<any, To> {
    template <typename X>
    static bool match(X&& x) {
        return x.type() == typeid(To);
    }

    template <typename X>
    static To convert(X&& x) {
        return any_cast<To>(std::forward<X>(x));
    }
};

using std::cout;
using std::endl;

// // Should fail to compile due to static_assert
// // because callback doesn't have arguments
// TEST(Visit_xxx) {
//     Visit(any(1),
//         [&]() { }
//     );
// }

// // Should fail to compile due to static_assert
// // because callback has a template argument
// TEST(Visit_xxx) {
//     Visit(any(1),
//         [&](auto x) {
//             TEST_EQ(x, 1);
//         }
//     );
// }

TEST(Visit_Single) {
    bool called = false;
    Visit(any(1),
        [&](int x) {
            called = true;
            TEST_EQ(x, 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByConstRef) {
    bool called = false;
    Visit(any(1),
        [&](const any& x) {
            called = true;
            TEST_EQ(any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByMutRef) {
    bool called = false;
    any x(1);
    Visit(x,
        [&](any& x) {
            called = true;
            TEST_EQ(any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByRRef) {
    bool called = false;
    Visit(any(1),
        [&](any&& x) {
            called = true;
            TEST_EQ(any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}
TEST(Visit_Single_ByValue) {
    bool called = false;
    Visit(any(1),
        [&](any x) {
            called = true;
            TEST_EQ(any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called);
}

TEST(Visit_Single_Self) {
    bool called = false;
    Visit(any(1.1),
        [&](double x, const any& any) {
            called = true;
            TEST_EQ(x, any_cast<double>(any));
        }
    );
    TEST_TRUE(called);
}

TEST(Visit_Single_MultipleCallbacks) {
    bool called_int = false;
    Visit(any(1),
        [&](int x) {
            called_int = true;
            TEST_EQ(x, 1);
        },
        [](unsigned) {
            TEST_FAIL;
        },
        [](const any&) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called_int);

    called_int = false;
    Visit(any(1),
        [](unsigned) {
            TEST_FAIL;
        },
        [&](int x) {
            called_int = true;
            TEST_EQ(x, 1);
        },
        [](const any&) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called_int);

    bool called_any = false;
    Visit(any(1),
        [](unsigned) {
            TEST_FAIL;
        },
        [&](const any& x) {
            called_any = true;
            TEST_EQ(any_cast<int>(x), 1);
        }
    );
    TEST_TRUE(called_any);
}

/*
TEST(Visit_anyInsideany) {
    any outer;
    any inner(2);
    outer = inner;
    cout << inner.type().name() << endl;
    cout << outer.type().name() << endl;
    TEST_EQ(any_cast<int>(inner), 2);
    TEST_EQ(any_cast<int>(outer), 2);

    bool called = false;
    Visit(outer,
        [&](const any& x) {
            called = true;
            TEST_EQ(any_cast<int>(x), 2);
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

/*
TEST(visit_vector) {
    std::vector<any> xs { 3, 2.5, 'D', 71, "Text" };

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
        [&i](const any&) {
            TEST_FAIL;
        }
    );
}
*/
