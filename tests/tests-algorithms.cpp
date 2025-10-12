#include "test.hpp"
#include STR(examples/EXAMPLE.hpp)

#include "VisitAlgorithms.hpp"

#include <map>

#ifndef EXAMPLE_INHERITANCE
#ifdef EXAMPLE_STD_ANY
        using std::any;
        using std::any_cast;
#elif defined EXAMPLE_BOOST_ANY
        using boost::any;
        using boost::any_cast;
#endif

TEST(FindFirstOneType) {
    const std::vector<any> many { 1.1, 2, 3.3f, 4, 'a', 5, "text" };
    auto* x = visit::FindFirst<int>(many);
    TEST_TRUE(x);
    TEST_EQ(any_cast<int>(*x), 2);
}

TEST(FindFirstTwoTypes) {
    const std::vector<any> many { 1.1, 2, 3.3f, 4, 'a', 5, "text" };
    auto* x = visit::FindFirst<char, float>(many);
    TEST_TRUE(x);
    TEST_EQ(any_cast<float>(*x), 3.3f);
}

TEST(FindFirstTwoTypesMap) {
    const std::map<std::string, any> many {
        { "1", 1.1 },
        { "2", 2 },
        { "3", 3.3f },
        { "4", 4 },
        { "5", 'a' },
        { "6", 5 },
        { "7", "text" }
    };
    auto* x = visit::FindFirst<char, float>(
        many,
        [](auto& x) -> auto& { return x.second; }
    );
    TEST_TRUE(x);
    TEST_EQ(any_cast<float>(*x), 3.3f);
}

TEST(ForEachMap) {
    std::map<std::string, any> many {
        { "1", 1.1 },
        { "2", 2 },
        { "3", 3.3f },
        { "4", 4 },
        { "5", 'a' },
        { "6", 5 },
        { "7", "text" }
    };
    visit::ForEach<double, float>(
        many,
        [](auto& x) { x = 0; }, // x is any
        [](auto& x) -> auto& { return x.second; }
    );
    TEST_EQ(any_cast<int>(many["1"]), 0);
    TEST_EQ(any_cast<int>(many["3"]), 0);
    TEST_EQ(any_cast<int>(many["2"]), 2);
    TEST_EQ(any_cast<int>(many["4"]), 4);
}
#endif
