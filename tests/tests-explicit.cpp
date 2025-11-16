#include "test.hpp"
#include STR(examples/EXAMPLE.hpp)

using visit::Visit;
using visit::VisitEach;

TEST(SingleMatching) {
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

TEST(SingleNotMatching) {
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
