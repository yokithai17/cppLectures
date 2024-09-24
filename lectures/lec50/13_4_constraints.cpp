#include <iostream>
#include <type_traits>

// 13.4 Constraints and requirements (since c++20)

template <typename T>
requires std::is_class_v<T>
    void f(const T&) {
    std::cout << 1;
}

template<typename T>
requires std::is_integral_v<T>
    void f(const T& x) {
    std::cout << 2;
}

template <typename T, size_t N>
requires (N <= 1000)
struct array {
    T arr[N];
};

int main() {


}