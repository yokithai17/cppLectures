#include <iostream>
#include <tuple>

// 11.4 Tuplees and strutered bingings.

int main() {
    std::tuple t{1, 2.0, 'a'};

    auto& d = std::get<1>(t);

    auto [x, y, z] = t; // copy
    auto& [x, y, z] = t; // lvalue reference
    auto&& [x, y, z] = t; // Универсальная ссылка
}