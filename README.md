# Overview
The Visit library implements type-based pattern matching for visiting objects
contained in type-erased wrappers such as `std::any`.

The goal is to provide ability to use concise syntax and strong type matching
for weakly-typed wrappers.

Requires C++17 or later standard.

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

To enable visiting a type, one needs to specialize the `struct VisitADL` template.
```c++
#include "visit.hpp"
#include <any>

template <typename To>
struct visit::VisitADL<std::any, To> {
    template <typename From>
    static bool match(From&& from) noexcept {
        return from.type() == typeid(To);
    }

    template <typename From>
    static To convert(From&& from) noexcept {
        return std::any_cast<To>(std::forward<From>(from));
    }
};
```
`From` designates the type being visited,
and `To` designates the type to which the `from` value will be converted.

With the overload in place, a `std::any` can be visited as follows:
```c++
std::any value(5);
visit::Visit(value,
    [](int x) {
        std::cout << x << std::endl;
    }
);
```
Containers are also supported.
```c++
std::vector<std::any> values(5, 1.2, 'a', std::string("text"));
visit::VisitEach(values, {},
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
4 kinds of visitor callbacks are supported,
differentiated by what arguments they take:
1. One argument matching the wrapper type,
```c++
Visit(std::any(5), [](const std::any& obj) { /* . . . */ });
```
2. One argument matching the wrapped type
```c++
Visit(std::any(5), [](int val) { /* . . . */ });
```
3. Two arguments, the second matching the wrapper type
```c++
Visit(std::any(5), [](int val, const std::any& obj) { /* . . . */ });
```
4. Two arguments, the first matching the wrapper type
```c++
Visit(std::any(5), [](const std::any& obj, int val) { /* . . . */ });
```
This is the order in which `Visit()` attempts to match callback signatures.
Priority is given to matching the wrapper type.

## Specializing `VisitADL`
To enable using `Visit()` and `VisitEach()` with a given type, one must
specialize the `struct VisitADL` template.
Note, that the base template is defined inside the `visit` namespace, and so
must be the specialization.

The first parameter of the template must be the *unqualified* type of the
wrapper type you want to visit.
The implementation passes here the type of the first argument of `Visit()`
after applying `std::decay_t<>`.
This approach makes it easier to specialize `VisitADL`, as the first parameter
does not need to account for all possible variations of `const`, `volatile`, and
reference qualifiers.
These variations can be accounted for by templating `match()` and `convert()`,
as in the `std::any` example.

The second parameter is the type that `Visit()` will attempt to retrieve from
the visited value.
This parameter is expected to be fully qualified.
The example with `std::any` illustrates that this is easier to achieve without
spelling out multiple distinct overloads.
This parameter is taken from the matching argument type of the callback.

## Chaining `match()` and `convert()`
For some types, matching and conversion are the same operation
(e.g. dynamic casting of pointers and references to polymorphic types),
or conversion cannot be done without checking (`any_cast` always checks).

The library provides a means to avoid redoing matching in `convert()`.

If `match()` returns anything other than `bool`, `Visit()` will chain the calls
to `match()` and `convert()` by forwarding the return value of `match()` as the
argument to `convert()` instead of forwarding `from` to `convert()`, i.e. the
same argument as `match()`.

The output of `match()` will still be used as a boolean condition to
check if the visited value matched the visitor argument type.
The type returned by `match()` must either be convertible to `bool`
or implement `bool operator!()`.

Using this feature, the `VisitADL` specialization can be implemented in the
following way:
```c++
template <typename To>
struct VisitADL<std::any, To> {
    template <typename From>
    static auto* match(From&& from) noexcept {
        return std::any_cast<std::remove_reference_t<To>>(&from);
    }

    template <typename Matched>
    static To convert(Matched* matched) noexcept {
        return static_cast<To>(*matched);
    }
};
```
The `std::any_cast()` overload taking a pointer to `std::any` instead of a
reference returns a null pointer on failure instead of throwing an exception.
Thus, the return of `match()` can be used as both a matching condition and
the argument for `convert()`.
This implementation avoids repeating the type check inevitably performed by
any version of `std::any_cast()`.

## Moving and forwarding
*Need to write.*

## `VisitEach`
The `VisitEach` function provides an means to visit each element of a container.

For example,
```c++
std::vector<std::any> many { 1, 2.f, 3., 'a', "text" };
VisitEach(many, {},
    [&](double x) { /* . . . */ },
    [&](const char* x) { /* . . . */ }
);
```
The implementation assumes the that container can be iterated over using a
range-based for loop.

### Projections
In many situations, the values that one would like to visit are wrapped in an
additional layer. One may have a container of pointers, or `std::pair`s, or
some user-defined classes with a visitable member.

The second argument of `VisitEach` is a projection function, whose purpose it
to unwrap extra layers of indirection.

For example,
```c++
std::vector<std::pair<std::string, std::any>> many { { "value", 5 } };
VisitEach(many, [](auto& pair) -> auto& { return pair.second; },
    [&](int value, const std::any& obj) { /* . . . */ }
);
```
The value returned by the projection function is forwarded to `Visit()`.

The `VisitEach()` template defaults to an identity projection, that forwards
its argument. The identity projection can be passed simply by writing `{}`.
There's no need to state its type explicitly.

### Break from callbacks
The library supports early breaking out of the `VisitEach()` loop.
Callback functions returning `bool` are treated as a special case.
Return values of any other type are discarded.

If an executed callback returns `true`, `VisitEach()` returns `true`,
immediately breaking the loop.
In all other cases, `VisitEach()` returns `false` after the loop is complete.

This behavior can be used to check if a container contains a certain wrapped
value.
For example,
```c++
std::vector<std::any> many { 1, "3", 3, "4", 5 };
if (VisitEach(many, {}, [](int x) { return x == 3; })) {
    std::puts("Found int 3!");
}
```
will check if `many` contains an `int` value of `3`, breaking the loop when
that value is found.

The ability to ask a loop if it exited normally is analogous to Python's
`for-else` construct.

# Measurements
## Compilation timing
Measurements of compilation time show that using this library adds 15 times less
compilation time than simply including the `<iostream>` header.

For a simple test, wrapping `dynamic_cast`
in [`tests/compilation-timing`](tests/compilation-timing),
using the library added 9 ± 1 ms to fully compiling an executable from a single
source file.

The following table summarizes compilation timing measurement from 100
compilation runs with `g++ -std=c++17 -Wall -O3`.

| Test               | Mean Time, ms | Stdev, ms |
|--------------------|--------------:|----------:|
| **Without visit**  |        64.840 | 0.950     |
|   With    visit    |        73.760 | 0.754     |
|   Difference       |         8.920 | 1.213     |
|   Include iostream |       202.330 | 1.045     |
|   Difference       |       137.490 | 1.413     |

## Optimization
In a simple test with `VisitADL<int, int>`
in [`tests/assembly`](tests/assembly)
wrapping an `int` and returning the same value back,
both `g++` and `clang++` optimize the entire abstraction away at `-O1`,
and generate assembly with a single instruction to return the `int` value.
