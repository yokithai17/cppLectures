#include <iostream>
#include <vector>
#include <iterator>

// 11.3 Class Template Argument Deduction (CTAD)
template <typename T>
class vector {
    template <typename Iter, Iter>
    vector(Iter, Iter) {}
};

template <typename Iter>
vector(Iter, Iter)
    -> vector<typename std::iterator_trairs<Iter>::value_type>;

int main() {
    std::vector v = {1, 2, 3, 4};

    std::vector v2{v.begin(), v.end()};
}