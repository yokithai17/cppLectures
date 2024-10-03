#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <algorithm>
#include <set>


// 14.2 Captures in lambdas


int main() {
    static int x = 0;

    auto f = [=]() mutable {
        ++x;
    };

    f();

    std::cout << x;
}