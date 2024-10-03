#include <algorithm>
#include <iostream>
#include <map>
#include <string>

// 14.4. Generalized (templated) lambdas


int main() {
  auto sum = [](int x, int y) { return x + y; };

  // fix_first_argument
  auto fix_second_argument = [](auto f, auto second_arg) {
    return [f, second_arg](auto first_arg) {
      return f(first_arg, second_arg);
    };
  };

  auto sum_with_five = fix_second_argument(sum, 5);

  // Currying (Карирование)
  std::cout << sum_with_five(3); // must be 8

  auto print_hello = []() {
    std::cout << "hello";
  };

  auto do_twice = [](auto&& f) {
    return [f = std::forward<decltype(f)>(f)] (const auto&...args) {
            f(args...);
            f(args...);
        };
    };

    do_twice(print_hello);

  // Recursive lambda (since c++23)

    auto fiboncci = [](this auto& self, int n) {
      return n > 2 ? self(n - 1) + self(n - 1) : 1;
    };
}