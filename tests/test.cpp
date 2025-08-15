#include <iostream>
#include <vector>
#include <any>
#include <ranges>
#include <concepts>

using std::cout, std::endl;

template <typename X, typename T>
concept DecaysTo = std::same_as<T, std::decay_t<X>>;

template <typename T, DecaysTo<std::any> X>
bool match(X&& x) noexcept {
    return x.type() == typeid(T);
}

template <typename T, DecaysTo<std::any> X>
T convert(X&& x) {
    return std::any_cast<T>(x);
}

#include "visit-14.hpp"

template <typename Xs, typename... F>
void VisitEach(Xs&& xs, F&&... callback)
requires requires {
  std::begin(xs) != std::end(xs);
}
{
    for (auto&& x : xs) {
        Visit(x, callback...);
    }
}

int main() {
  std::ios_base::sync_with_stdio(false);

  std::vector<std::any> xs { 1, 2, 3.4, 'D', "Text" };

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
