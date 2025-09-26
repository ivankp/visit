#include "visit.hpp"
#include <utility>
#include <type_traits>

template <typename T, typename C>
struct const_like {
  using type = T;
};

template <typename T, typename C>
struct const_like<T, const C> {
  using type = const T;
};

template <typename T, typename C>
using const_like_t = typename const_like<T,C>::type;

struct Base {
    virtual ~Base() = default;
};

template <typename T>
struct Derived : Base {
    T value;
    template <typename... Args>
    Derived(Args&&... args): value(std::forward<Args>(args)...) { }
};

template <typename To>
struct VisitADL<Base, To> {
private:
    template <typename From>
    static auto* cast(From* from) noexcept {
        return dynamic_cast<
            const_like_t<Derived<std::decay_t<To>>, From>*
        >(from);
    }

public:
    template <typename From>
    static bool match(From&& from) noexcept {
        return cast(&from);
    }

    template <typename From>
    static To convert(From&& from) noexcept {
        return static_cast<To>(cast(&from)->value);
    }
};

#define EXAMPLE_INHERITANCE
