#include <vector>
#include <utility>
#include <iostream>

std::vector<std::pair<void(*)(), const char*>> allTests;

struct RegisterTest {
    RegisterTest(void(*f)(), const char* name) {
        allTests.emplace_back(f,name);
    }
};

int main() {
    using std::cout;
    using std::endl;

    for (const auto& test : allTests) {
        cout << "\033[34m[ Test   ]\033[0m " << test.second << "\n";
        try {
            (*test.first)();
            cout << "\033[32m[ PASSED ]\033[0m " << test.second << "\n";
        } catch (const std::exception& e) {
            cout << e.what() << endl;
            cout << "\033[31m[ FAILED ]\033[0m " << test.second << "\n";
        }
    }
}

#define TEST(NAME) \
    void NAME(); \
    RegisterTest RegisterTest_##NAME(&NAME, #NAME); \
    void NAME()

#define TEST_EQ(LHS, RHS) \
    if (!( (LHS) == (RHS) )) { \
        throw std::runtime_error(#LHS " != " #RHS); \
    }

#define TEST_TRUE(X) \
    if (!(X)) { \
        throw std::runtime_error(#X " is false"); \
    }

#define TEST_FALSE(X) \
    if ((X)) { \
        throw std::runtime_error(#X " is true"); \
    }
