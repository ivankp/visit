# Overview
The Visit library implements type-based pattern matching for visiting objects
contained in type-erased wrappers such as `std::any`.

Requires at least C++17.

## Benefits
* Improved type safety with type matching.
  * Stringly typed lookups can be converted to type-checked visits.
* Reduced amount of boilerplate code.
  * Write matching code only once rather than at every usage place.
  * `VisitEach()` wraps iteration over containers.
* Terse usage syntax.
* Specializing a simple template provides consistent and rich behavior.

## Integration
Copy two header files,
[callable.hpp](include/callable.hpp) and [visit.hpp](include/visit.hpp)
into your project.

[callable.hpp](include/callable.hpp) can be used independently
for type traits that can retrieve return and argument types of callables.

# Example
An archetypal example of a class that can be used with the Visit library is
[`std::any`](https://en.cppreference.com/w/cpp/utility/any.html).

To anable visiting a type, one needs to specialize the `struct VisitADL` template.
```c++
#include "visit.hpp"
#include <any>

template <typename To>
struct VisitADL<std::any, To> {
    template <typename From>
    static bool match(From&& from) {
        return from.type() == typeid(To);
    }

    template <typename From>
    static To convert(From&& from) {
        return std::any_cast<To>(std::forward<From>(from));
    }
};
```
`From` designates the type being visited,
and `To` designates the type to which the `from` value will be converted.

With the overload in place, an `std::any` can be visited as follows:
```c++
std::any value(5);
Visit(value,
    [](int x) {
        std::cout << x << std::endl;
    }
);
```
Containers are also supported.
```c++
std::vector<std::any> values(5, 1.2, 'a', std::string("text"));
VisitEach(values, {},
    [](int i) {
        std::cout << "int " << i << std::endl;
    },
    [](double d) {
        std::cout << "double " << d << std::endl;
    },
    [](char c) {
        std::cout << "char " << c << std::endl;
    },
    [](const std::string& s) {
        std::cout << "string " << s << std::endl;
    }
);
```
The first callback for which the argument type matches the type of the wrapped object
is the one that is called in each case.

# Documentation
## Visitor callback signatures

## Specializing `VisitADL`

## Chaining `match()` and `convert()`
For some types, matching and conversion are the same operation
(e.g. dynamic casting of pointers and references to polymorphic types),
or conversion cannot be done without checking (`any_cast` always checks).

The library provides a means to avoid redoing matching in `convert()`.

If `match()` returns anything other than `bool`, `Visit()` will chain the calls
to `match()` and `convert()` by forwarding the return value of `match()` as the
argument to `convert()` instead of calling `convert()` on the visited value
(the first argument of `Visit()`).

The output of `match()` will still be used as a boolean condition to
check if the visited value matched the visitor argument type.
The type returned by `match()` must either be convertable to `bool`
or implement `bool operator!()`.

## Moving and forwarding

## `VisitEach` and projections
