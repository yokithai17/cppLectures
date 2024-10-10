#include <iostream>
#include <variant>

// 15.2 std::variant (since c++17)
// 15.3 std::visit

int main() {
    
    std::varint<int, double, std::string> v = 5;

    std::cout << std::get<int>(v);

    v = 3.14;

    std::cout << std::get<double>(v);

    v = "abs";

    std::cout << std::get<std::string>(v);

std::visit([](const auto& item) -> int {
            if constexpr (std::is_same_v<decltype(item), int>) {
                std::cout << "int! " << item << '\n';
            } else {
                st::cout << "not int\n";
            }

            return 0;
        }, v);
}
