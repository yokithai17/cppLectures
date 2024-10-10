#include <variant>
#include <string>
#include <type_traits>
#include <vector>

// OVerload pattern

template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...; // since c++17
};

template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

int main() {
    std::vector<std::variant<std::vector<int>, double, std::string>>
        vecVariant = { 1.5, std::vector<int>{1, 2, 3, 4, 5}, "HELOO"};

    auto DisplayMe = Overload {
        [](std::vector<int>& myVec) {
            for (auto v : myVec) std::cout << v << ' ';
            std::cout << '\n';
        },
        [](auto& arg) {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string) {
                std::cout << "STRNG " << arg << '\n';
            } else if constexpr(std::is_same_v<std::decay_t<decltype(arg)>, double) {
                std::cout << "DEOUBLE " << arg << '\n';
            }
        },
    };

    for (auto v : vecVariant) {
        std::visit(DisplayMe, v);
    }
}