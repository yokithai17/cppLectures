#include <iostream>
#include <concepts>

template <typename T>
concept InputeIter = requires(T x) {
    *x;
    ++x;
};


template <typename T>
concept ForwardIter = InputeIter && requires(T x) {
    x++;
};


template <typename T>
concept BiderIter = ForwardIter && requires(T x) {
    --x;
    x--;
};

template <typename T>
concept RandomIter = ForwardIter && requires(T x, T y) {
    x - y;
    x < y; x > y; x <= y; x >= y;
    x -= 1; x += 1;
    x + 1; x - 1; 1 + x;
};



template <std::input_iterator Iter>
void advance(Iter& iter, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ++iter;
    }
}

template <std::random_access_iterator Iter>
void advance(Iter& iter, size_t n) {
    iter += n;
}


int main() {

}