#include "test.hpp"
#include "visit-inheritance.hpp"

TEST(CorrectType1) {
    Base&& x = Derived<int>(113);

    bool called = false;
    Visit(x,
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(WrongType1) {
    Base&& x = Derived<int>(113);

    bool called = false;
    Visit(x,
        [&](double) {
            called = true;
        }
    );
    TEST_FALSE(called);
}

TEST(CorrectTypeConst) {
    const Base& x = Derived<int>(113);

    bool called = false;
    Visit(x,
        [&](int x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(CorrectTypeRef) {
    Base&& x = Derived<int>(113);

    bool called = false;
    Visit(x,
        [&](int& x) {
            called = true;
            TEST_EQ(x, 113);
            x = 17;
        }
    );
    TEST_TRUE(called);
    TEST_EQ(static_cast<Derived<int>&>(x).value, 17);
}

TEST(CorrectTypeConstRef) {
    const Base& x = Derived<int>(113);

    bool called = false;
    Visit(x,
        [&](const int& x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}

TEST(Fail) {
    const Base& x = Derived<int>(113);

    bool called = false;
    Visit(x,
        [&](int& x) {
            called = true;
            TEST_EQ(x, 113);
        }
    );
    TEST_TRUE(called);
}
