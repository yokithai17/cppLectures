#include <iostream>

// 11.2 decltype keyword

template <typename T>
struct Debug {
    Debug() = delete;
};

int main() {
    int x = 0;
    decltype((x)) y = x;
}