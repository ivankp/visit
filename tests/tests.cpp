#include "test.hpp"
#include STR(../examples/EXAMPLE.hpp)

#define CAT1(A, B) A##B
#define CAT(A, B) CAT1(A, B)

#define I_inheritance 1
#define INHERITANCE CAT(I_, EXAMPLE) == I_inheritance

#define I_std_any 2
#define STD_ANY CAT(I_, EXAMPLE) == I_std_any

#define I_boost_any 3
#define BOOST_ANY CAT(I_, EXAMPLE) == I_boost_any

TEST(MutableByValue) {
#if INHERITANCE
    Base&& x = Derived<int>(113);
#elif STD_ANY
    std::any x(113);
#elif BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(WrongType) {
#if INHERITANCE
    Base&& x = Derived<int>(113);
#elif STD_ANY
    std::any x(113);
#elif BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](double) {
            called = true;
        }
    );
    TEST_FALSE(called);
}

TEST(ConstByValue) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(MutableByRef) {
#if INHERITANCE
    Base&& x = Derived<int>(113);
#elif STD_ANY
    std::any x(113);
#elif BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int& x) {
            called = true;
            TEST_EQ(x, 113);
            x = 17;
        }
    );
    TEST_TRUE(called);

#if INHERITANCE
    TEST_EQ(static_cast<Derived<int>&>(x).value, 17);
#elif STD_ANY
    TEST_EQ(std::any_cast<int>(x), 17);
#elif BOOST_ANY
    TEST_EQ(boost::any_cast<int>(x), 17);
#endif
}

TEST(MutableByConstRef) {
#if INHERITANCE
    Base&& x = Derived<int>(113);
#elif STD_ANY
    std::any x(113);
#elif BOOST_ANY
    boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](const int& x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(ConstByConstRef) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](const int& x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

// -----------------------------------------------------------------------------

TEST(TwoVisitorsFirst) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        },
        [&](float) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);
}

TEST(TwoVisitorsSecond) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](float) {
            TEST_FAIL;
        },
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

// -----------------------------------------------------------------------------

TEST(FromArg) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#if INHERITANCE
        [&](const Base&)
#elif STD_ANY
        [&](const std::any&)
#elif BOOST_ANY
        [&](const boost::any&)
#endif
        {
            called = true;
        }
    );
    TEST_TRUE(called);
}

TEST(FromArgBeforeToArg) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#if INHERITANCE
        [&](const Base&)
#elif STD_ANY
        [&](const std::any&)
#elif BOOST_ANY
        [&](const boost::any&)
#endif
        {
            called = true;
        },
        [&](int) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);
}

TEST(FromArgAfterToArg) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int) {
            called = true;
        },
#if INHERITANCE
        [&](const Base&)
#elif STD_ANY
        [&](const std::any&)
#elif BOOST_ANY
        [&](const boost::any&)
#endif
        {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);
}

TEST(TwoArgs1) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](float) {
            TEST_FAIL;
        },
#if INHERITANCE
        [&](int x, const Base&)
#elif STD_ANY
        [&](int x, const std::any&)
#elif BOOST_ANY
        [&](int x, const boost::any&)
#endif
        {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(TwoArgs2) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#if INHERITANCE
        [&](const Base&, int x)
#elif STD_ANY
        [&](const std::any&, int x)
#elif BOOST_ANY
        [&](const boost::any&, int x)
#endif
        {
            called = true;
            TEST_EQ(x, 113);
        },
        [&](float) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);
}

TEST(TwoArgs3) {
#if INHERITANCE
    const Base& x = Derived<int>(113);
#elif STD_ANY
    const std::any x(113);
#elif BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#if INHERITANCE
        [&](float, const Base&)
#elif STD_ANY
        [&](float, const std::any&)
#elif BOOST_ANY
        [&](float, const boost::any&)
#endif
        {
            TEST_FAIL;
        },
#if INHERITANCE
        [&](int x, const Base&)
#elif STD_ANY
        [&](int x, const std::any&)
#elif BOOST_ANY
        [&](int x, const boost::any&)
#endif
        {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}
