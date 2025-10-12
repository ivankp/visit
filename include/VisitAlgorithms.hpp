// https://github.com/ivankp/visit

// clang-format off
#pragma once

#include <Visit.hpp>

namespace visit {

// template <typename Container, typename Proj>
// constexpr auto* ElementPointer() noexcept {
//     using std::begin;
//     return static_cast<std::remove_reference_t<
//         decltype(std::declval<Proj>()(*begin(std::declval<Container>())))
//     >*>(nullptr);
// }

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
    /*
    for (auto& element : static_cast<Container&&>(container)) {
        auto& x = proj(element);
        if ((
            VisitADL<std::decay_t<decltype(element)>, T>::match(x)
            || ...
        )) {
            ptr = &x;
            break;
        }
    }
    */
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
