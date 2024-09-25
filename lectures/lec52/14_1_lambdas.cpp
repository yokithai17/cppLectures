#include <iostream>
#include <vector>
#include <algorithm>

// XIV. Lamdas. closures and elemetns of functional programming

// 14.1 Basic usages of lambdas.


int main() {
    // Closure
    auto cmp = [](int x, int y) -> int
    { return x + y;};

    [](){}();

    const int x = [](int y) {
        if (y % 2 == 0) {
            return y / 2;
        } else {
            return 3 * y + 1;
        }
    }(12);

    
}