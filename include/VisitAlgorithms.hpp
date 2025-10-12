// https://github.com/ivankp/visit

// clang-format off
#pragma once

#include <Visit.hpp>

namespace visit {

template <
    typename... T,
    typename Container,
    typename Proj = detail::identity
>
auto* FindFirst(Container&& container, Proj proj = { }) {
    using std::begin;
    using Element = std::remove_reference_t<
        decltype(proj(*begin(std::declval<Container>())))
    >;

    Element* ptr = nullptr;
    VisitEach(static_cast<Container&&>(container), proj,
        [&](Element& element, WrapsEither<T...>) {
            ptr = &element;
            return true; // break VisitEach loop
        }
    );
    return ptr;
}

template <
    typename... T,
    typename Container,
    typename Callback,
    typename Proj = detail::identity
>
void ForEach(Container&& container, Callback&& callback, Proj proj = {}) {
    using std::begin;
    using Element = decltype(proj(*begin(std::declval<Container>())));

    VisitEach(static_cast<Container&&>(container), proj,
        [&](Element&& element, WrapsEither<T...>) {
            callback(static_cast<Element&&>(element));
        }
    );
}

}
