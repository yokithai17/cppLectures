#include <iostream>
#include <functional>

// 14.6 std::bind, bind-object

int sum(int x, int y) {
    return x + y;
}

int diff(int x, int y) {
    return x - y;
}


int main() {
    auto f = std::bind(sum, 5, std::placeholders::_1);

    auto g = std::bind(diff, std::placeholders::_1, 5);
    std::cout << f(3) << ' ' << g(3);
}