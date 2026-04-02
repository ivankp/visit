#include "test.hpp"
#include STR(examples/EXAMPLE.hpp)

using visit::Visit;
using visit::VisitEach;

TEST(SingleAutoMatching) {
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit<int>(x,
        [&](auto x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(SingleAutoNotMatching) {
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit<double>(x,
        [&](auto x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_FALSE(called);
}

TEST(Conversion) {
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit<int>(x,
        [&](double x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

/*
TEST(Test) {
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit<int>(x,
        [&](const char*) {
            called = true;
        }
    );
    TEST_TRUE(called);
}
*/

#ifndef EXAMPLE_INHERITANCE

TEST(VisitEachProj) {
    const std::vector<std::pair<
        const char*,
#ifdef EXAMPLE_STD_ANY
        std::any
#elif defined EXAMPLE_BOOST_ANY
        boost::any
#endif
    >> many {
        { "int"   , 1      },
        { "float" , 2.f    },
        { "double", 3.     },
        { "char"  , 'a'    },
        { "string", "text" }
    };

    int i = 0;
    int called[5] { };

    VisitEach<double, float>(many,
        [](const auto& x) -> const auto& { return x.second; },
        [&](auto x) {
            ++called[++i];
            if constexpr (std::is_same_v<decltype(x), float>) {
                TEST_EQ(x, 2.f);
            } else if constexpr (std::is_same_v<decltype(x), double>) {
                TEST_EQ(x, 3.0);
            }
        }
    );

    TEST_EQ(i, 2);
    TEST_EQ(called[0], 0);
    TEST_EQ(called[1], 1);
    TEST_EQ(called[2], 1);
    TEST_EQ(called[3], 0);
    TEST_EQ(called[4], 0);
}

#endif
