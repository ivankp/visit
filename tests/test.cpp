#include "test.hpp"

#include <vector>
#include <utility>
#include <type_traits>

using std::cout;
using std::endl;

size_t NewTypeIndex() {
    static size_t index = 0;
    return index++;
}

template <typename T>
size_t GetTypeIndex() {
    static const size_t index = NewTypeIndex();
    return index;
}

class Any;

template <typename T>
T Get(Any& any);
template <typename T>
T Get(const Any& any);
template <typename T>
bool Has(const Any& any) noexcept;

class Any {
    size_t typeIndex;
    void* data;
    void(*destructor)(void*);

public:
    template <typename T>
    Any(T&& x)
    : typeIndex(GetTypeIndex<std::decay_t<T>>()),
      data(reinterpret_cast<void*>(new std::decay_t<T>(std::forward<T>(x)))),
      destructor([](void* ptr){
          using type = std::decay_t<T>;
          reinterpret_cast<type*>(ptr)->~type();
      })
    { }
    ~Any() {
        (*destructor)(data);
    }

    template <typename T>
    friend T Get(Any& any);
    template <typename T>
    friend T Get(const Any& any);
    template <typename T>
    friend bool Has(const Any& any) noexcept;
};

template <typename T>
T Get(Any& any) {
    using type = std::decay_t<T>;
    if (any.typeIndex == GetTypeIndex<type>()) {
        return *reinterpret_cast<type*>(any.data);
    } else {
        throw std::runtime_error("This instance of Any isn't holding this type");
    }
}
template <typename T>
T Get(const Any& any) {
    using type = std::decay_t<T>;
    if (any.typeIndex == GetTypeIndex<type>()) {
        return *reinterpret_cast<const type*>(any.data);
    } else {
        throw std::runtime_error("This instance of Any isn't holding this type");
    }
}

template <typename T>
bool Has(const Any& any) noexcept {
    using type = std::decay_t<T>;
    return any.typeIndex == GetTypeIndex<type>();
}

TEST(Any_CorrectType) {
    TEST_EQ(Get<int>(Any(5)), 5);
    TEST_EQ(Get<double>(Any(2.3)), 2.3);
}

TEST(Any_IncorrectType) {
    bool exception = false;
    try {
        Get<double>(Any(5));
    } catch (...) {
        exception = true;
    }
    TEST_EQ(exception, true);

    try {
        Get<int>(Any(2.3));
    } catch (...) {
        exception = true;
    }
    TEST_EQ(exception, true);
}

TEST(Any_Has) {
    Any x(1);
    TEST_TRUE(Has<int>(x));
    TEST_FALSE(Has<unsigned>(x));
}

#if 0
template <typename X, typename T>
concept DecaysTo = std::same_as<T, std::decay_t<X>>;

template <typename T, DecaysTo<Any> X>
bool match(X&& x) noexcept {
    return Has<T>(x);
}

template <typename T, DecaysTo<Any> X>
T convert(X&& x) {
    return Get<T>(x);
}

#else

template <typename X, typename T>
constexpr bool decaysTo = std::is_same<T, std::decay_t<X>>::value;

template <typename T, typename X>
auto match(X&& x) noexcept -> std::enable_if_t<decaysTo<X, Any>, bool> {
    return Has<T>(x);
}

template <typename T, typename X>
auto convert(X&& x) noexcept -> std::enable_if_t<decaysTo<X, Any>, T> {
    return Get<T>(x);
}
#endif

#if __cplusplus >= 202002L
#include "visit-cpp20.hpp"
#else
#include "visit-cpp14.hpp"
#endif

TEST(Visit_match_Any) {
    Any x(1);
    TEST_TRUE(match<int>(x));
    TEST_FALSE(match<unsigned>(x));
}

TEST(Visit_convert_Any) {
    TEST_EQ(convert<int>(Any(5)), 5);
    TEST_EQ(convert<double>(Any(2.3)), 2.3);
}

TEST(Visit_SingleValue) {
    Any x(1);

    Visit(x,
        [](int) {
        },
        [](const Any&) {
            throw std::runtime_error("This shouldn't happen");
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
    std::vector<Any> xs { 1, 2, 3.4, 'D', "Text" };

    VisitEach(xs,
        [](int x) {
            cout << x << endl;
        },
        [](const char* x) {
            cout << x << endl;
        },
        [](double x) {
            cout << x << endl;
        }
    );
}
