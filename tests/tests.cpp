#include "test.hpp"
#include STR(examples/EXAMPLE.hpp)

using visit::Visit;
using visit::VisitEach;

TEST(MutableByValue) {
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
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

#ifdef EXAMPLE_INHERITANCE
    TEST_EQ(static_cast<Derived<int>&>(x).value, 17);
#elif defined EXAMPLE_STD_ANY
    TEST_EQ(std::any_cast<int>(x), 17);
#elif defined EXAMPLE_BOOST_ANY
    TEST_EQ(boost::any_cast<int>(x), 17);
#endif
}

TEST(MutableByConstRef) {
#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
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

TEST(ThreeCallbacksFirst) {
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        },
        [&](const int*) {
            TEST_FAIL;
        },
        [&](float) {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);
}

TEST(ThreeCallbacksSecond) {
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](const int*) {
            TEST_FAIL;
        },
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

TEST(ThreeCallbacksThird) {
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](const int*) {
            TEST_FAIL;
        },
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
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#ifdef EXAMPLE_INHERITANCE
        [&](const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](const std::any&)
#elif defined EXAMPLE_BOOST_ANY
        [&](const boost::any&)
#endif
        {
            called = true;
        }
    );
    TEST_TRUE(called);
}

TEST(FromArgBeforeToArg) {
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#ifdef EXAMPLE_INHERITANCE
        [&](const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](const std::any&)
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](int) {
            called = true;
        },
#ifdef EXAMPLE_INHERITANCE
        [&](const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](const std::any&)
#elif defined EXAMPLE_BOOST_ANY
        [&](const boost::any&)
#endif
        {
            TEST_FAIL;
        }
    );
    TEST_TRUE(called);
}

TEST(TwoArgs1) {
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](float) {
            TEST_FAIL;
        },
#ifdef EXAMPLE_INHERITANCE
        [&](int x, const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](int x, const std::any&)
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#ifdef EXAMPLE_INHERITANCE
        [&](const Base&, int x)
#elif defined EXAMPLE_STD_ANY
        [&](const std::any&, int x)
#elif defined EXAMPLE_BOOST_ANY
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
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
#ifdef EXAMPLE_INHERITANCE
        [&](float, const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](float, const std::any&)
#elif defined EXAMPLE_BOOST_ANY
        [&](float, const boost::any&)
#endif
        {
            TEST_FAIL;
        },
#ifdef EXAMPLE_INHERITANCE
        [&](int x, const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](int x, const std::any&)
#elif defined EXAMPLE_BOOST_ANY
        [&](int x, const boost::any&)
#endif
        {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

// -----------------------------------------------------------------------------

TEST(FromAfterBadTo) {
#ifdef EXAMPLE_INHERITANCE
    const Base& x = Derived<int>(113);
#elif defined EXAMPLE_STD_ANY
    const std::any x(113);
#elif defined EXAMPLE_BOOST_ANY
    const boost::any x(113);
#endif

    bool called = false;
    Visit(x,
        [&](const int*) {
            TEST_FAIL;
        },
#ifdef EXAMPLE_INHERITANCE
        [&](const Base&)
#elif defined EXAMPLE_STD_ANY
        [&](const std::any&)
#elif defined EXAMPLE_BOOST_ANY
        [&](const boost::any&)
#endif
        {
            called = true;
        }
    );
    TEST_TRUE(called);
}

// Order of preference ---------------------------------------------------------

#ifdef EXAMPLE_INHERITANCE

// Need to specialize to avoid decaying the Derived<> parameter
namespace visit {
template <>
struct VisitADL<Base, const Base&> {
private:
    template <typename From>
    static auto* cast(From* from) noexcept {
        return dynamic_cast<
            const_like_t<Derived<const Base&>, From>*
        >(from);
    }

public:
    template <typename From>
    static bool match(From&& from) noexcept {
        return cast(&from);
    }

    template <typename From>
    static const Base& convert(From&& from) noexcept {
        return cast(&from)->value;
    }
};
}

TEST(PreferFromArgSingle) {
    // Test that a non-converting *from* arg takes precedence over
    // a converting *to* arg.

    const Base& a = Derived<int>(113);
    const Base& b = Derived<const Base&>(a);

    bool called = false;
    Visit(b,
        [&](const Base& from) {
            if (!dynamic_cast<const Derived<const Base&>*>(&from)) {
                TEST_FAIL;
            }
            if (dynamic_cast<const Derived<int>*>(&from)) {
                TEST_FAIL;
            }
            called = true;
        }
    );
    TEST_TRUE(called);
}

TEST(PreferFromArgRight) {
    // Test that a non-converting *from* arg on the right takes precedence over
    // the same on the left.

    const Base& a = Derived<int>(113);
    const Base& b = Derived<const Base&>(a);

    bool called = false;
    Visit(b,
        [&](const Base& to, const Base& from) {
            if (!dynamic_cast<const Derived<int>*>(&to)) {
                TEST_FAIL;
            }
            if (!dynamic_cast<const Derived<const Base&>*>(&from)) {
                TEST_FAIL;
            }
            if (dynamic_cast<const Derived<int>*>(&from)) {
                TEST_FAIL;
            }
            if (dynamic_cast<const Derived<const Base&>*>(&to)) {
                TEST_FAIL;
            }
            called = true;
        }
    );
    TEST_TRUE(called);
}
#endif

// -----------------------------------------------------------------------------

#include <vector>

TEST(Move) {
    std::vector<int> vec { 1,2,3,4,5 };

#ifdef EXAMPLE_INHERITANCE
    Base&& x = Derived<std::vector<int>>(std::move(vec));
#elif defined EXAMPLE_STD_ANY
    std::any x(std::move(vec));
#elif defined EXAMPLE_BOOST_ANY
    boost::any x(std::move(vec));
#endif

    TEST_TRUE(vec.empty());

    bool called = false;
    Visit(std::move(x),
        [&](std::vector<int>&& x) {
            called = true;
            vec = std::move(x);
        }
    );
    TEST_TRUE(called);
    TEST_EQ(vec.size(), 5);

#ifdef EXAMPLE_INHERITANCE
    TEST_TRUE(static_cast<Derived<std::vector<int>>&>(x).value.empty());
#elif defined EXAMPLE_STD_ANY
    TEST_TRUE(std::any_cast<const std::vector<int>&>(x).empty());
#elif defined EXAMPLE_BOOST_ANY
    TEST_TRUE(boost::any_cast<const std::vector<int>&>(x).empty());
#endif
}

// -----------------------------------------------------------------------------

#include <string_view>

#ifndef EXAMPLE_INHERITANCE

TEST(VisitEach) {
    const std::vector<
#ifdef EXAMPLE_STD_ANY
        std::any
#elif defined EXAMPLE_BOOST_ANY
        boost::any
#endif
    > many { 1, 2.f, 3., 'a', "text" };

    int i = 0;
    int called[5] { };

    VisitEach(many, {},
        [&](double x) {
            called[2] = ++i;
            TEST_EQ(x, 3.);
        },
        [&](float x) {
            called[1] = ++i;
            TEST_EQ(x, 2.f);
        },
        [&](int x) {
            called[0] = ++i;
            TEST_EQ(x, 1);
        },
        [&](char x) {
            called[3] = ++i;
            TEST_EQ(x, 'a');
        },
        [&](std::string_view) {
            TEST_FAIL;
        },
        [&](const char* x) {
            called[4] = ++i;
            TEST_EQ(std::string_view(x), "text");
        }
    );
    TEST_EQ(i, 5);
    TEST_EQ(called[0], 1);
    TEST_EQ(called[1], 2);
    TEST_EQ(called[2], 3);
    TEST_EQ(called[3], 4);
    TEST_EQ(called[4], 5);
}

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

    VisitEach(many, [](const auto& x) -> const auto& { return x.second; },
        [&](double x) {
            called[2] = ++i;
            TEST_EQ(x, 3.);
        },
        [&](float x) {
            called[1] = ++i;
            TEST_EQ(x, 2.f);
        },
        [&](int x) {
            called[0] = ++i;
            TEST_EQ(x, 1);
        },
        [&](char x) {
            called[3] = ++i;
            TEST_EQ(x, 'a');
        },
        [&](std::string_view) {
            TEST_FAIL;
        },
        [&](const char* x) {
            called[4] = ++i;
            TEST_EQ(std::string_view(x), "text");
        }
    );
    TEST_EQ(i, 5);
    TEST_EQ(called[0], 1);
    TEST_EQ(called[1], 2);
    TEST_EQ(called[2], 3);
    TEST_EQ(called[3], 4);
    TEST_EQ(called[4], 5);
}

TEST(VisitEachControl) {
#ifdef EXAMPLE_STD_ANY
        using std::any;
#elif defined EXAMPLE_BOOST_ANY
        using boost::any;
#endif
    const std::vector<any> many { 1.1, 2, 3.3f, 4, 'a', 5, "text" };

    int value = 0;
    int count = 0;
    const bool found = VisitEach(many, {},
        [&](int x) -> bool { // only returning bool counts as control
            value = x;
            return x == 4;
        },
        [&](const any&) {
            ++count;
        }
    );
    TEST_TRUE(found);
    TEST_NE(value, 2);
    TEST_NE(value, 5);
    TEST_EQ(value, 4);
    TEST_EQ(count, 2); // ints are not counted
}

TEST(VisitEachNotControl) {
#ifdef EXAMPLE_STD_ANY
        using std::any;
#elif defined EXAMPLE_BOOST_ANY
        using boost::any;
#endif
    const std::vector<any> many { 1.1, 2, 3.3f, 4, 'a', 5, "text" };

    int value = 0;
    int count = 0;
    const bool found = VisitEach(many, {},
        [&](int x) -> bool& { // even bool& doesn't count as control
            value = x;
            static bool b;
            b = x == 4;
            return b;
        },
        [&](const any&) {
            ++count;
        }
    );
    TEST_FALSE(found);
    TEST_NE(value, 2);
    TEST_NE(value, 4);
    TEST_EQ(value, 5);
    TEST_EQ(count, 4); // ints are not counted
}
#endif
