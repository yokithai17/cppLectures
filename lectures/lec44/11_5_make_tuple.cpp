#include <iostream>
#include <tuple>

template <typename T>
struct X {
    explicit X(const Ts&... values)
        : values(std::make_tuple(values...)) {}

    std::tuple<const Ts&...> values;
};

int main() {
    int i = 42;
    auto s = std::string("dsSAD");

    auto x = X<int, std::string>(i, s);

    std::cout << "i = " << std::get<0>(x.values);
    std::cout << "s = " << std::get<1>(x.values);
}