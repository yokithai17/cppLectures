#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <algorithm>
#include <set>


// 14.2 Captures in lambdas

struct S {
    int a = 0;

    auto getLambda() {
        auto f = [=](int x) {
            return x + a;
        };
    }
};





template <typename... Strings>
requires ((std::is_same_v<Strings, std::string> && ...))
    void test(const Strings&... subs) {
    auto cn_s = [&subs...](const std::string& str) {
        return ((str.find(subs) != std::string::npos) && ...);
    };
}


int main() {
    std::vector<std::string> v = {"dsa", "bavae", "fefeaa"};

    std::string sub = "av";

    auto con_sub = [&sub](const std::string& str) {
        return str.find(sub) != std::string::npos;
    };

    auto iter = std::find_if(v.begin(), v.end(),
    con_sub);

    auto cmp = [](int x, int y) mutable {
        return x * x < y * y;
    };

    std::set<int, decltype(cmp)> s = {1, 2, 3, 4};
    
}