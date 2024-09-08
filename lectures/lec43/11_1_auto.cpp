#include <iostream>

// XI. Type deduction
//
// 11.1. auto keyword

template <typename T>
void f(const T& x) {}

template <typename Container>
void f(Container&& container) {
    for (auto&& value: std::forward<Container>(container)) {
        // ...
    }
}

template <typename T>
auto g(T x) {
    return ++x;
}

int main() {
    auto x = 5;
    auto& y = x;
    const auto& z = y;
    auto&& t = x;
    auto& a = 0; // CE
    auto&& b = 0; // OK life time 
    std::vector<auto> v = {1, 2}; // CE
    int* p = new auto(5);
}
