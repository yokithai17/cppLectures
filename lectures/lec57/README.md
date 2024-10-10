# Лекция 57. Unions, std::variant, std::visit. Overload pattern

## Unions

Размер юниона это максимум из размерности полей в точности до выравнивание. Она хранит в моменте, что то одно из полей. Потому что они лежат на 1 адрессе

```c++
union U {
    int x;
    double y;
};

int main() {
    U u;
    U.x = 5;
    u.y = 3.14;
    std::cout << u.x; // тут будет что то другое а не 5
}
```

Интересней становиться когда мы кладем не тривиальный тип. К примеру строку. Вообще у нас здесь будет СЕ так как когда мы имеем в полях что то не тривиальное, что не можем дефолтный. Давайте просто возьмем и сделаем какие то

```c++
union U {
    int x;
    double y;
    std::string;

    U() {}
    ~U() {}
};

U.s = "Abc" // CORE DUMPED
```

ВОт и правильный способ положить, что то в Union. Но теперь проблема, а что если мы хотим теперь делать присвавагие Int.

```c++
new (&U.s) std::string("abs");
// U.x = 5;  memory leak
u.s.~basic_string<char>();
u.x = 5;
```

## member lifetime

По умолчанию никакой мембер не активен. Как нам к примеру положить const int => placement new

## anonumous union

```c++
int main() {
    union {
        int a;
        const char* p;
    };
    a = 1;
    p = "HELLO";
}
```

А где же изпользуется union? ОКАЗЫВАЕТСЯ, что в std::string изпользуется union. ЧЗХ???)))

## SSO = small string optimization

Если у нас маленькая строка, то зачем нам хранить ее на куче, когда можно это сделать на стеке.

```c++
union
{
    charT     _M_local_buf[cap + 1];
    size_type _M_allocated_cap;
};
```

Давайте теперь вспомним std::function как там можно оптимизировать... В стл-м нету((.

мы можем вместо буффера локального хранить delete_ptr?

## std::variant

Ну невозможно же пользоваться union. Это же ппросто расадник UB.

ДАвайте сделаем обретку над union

```c++
int main() {
    
    std::varint<int, double, std::string> v = 5;

    std::cout << std::get<int>(v);

    v = 3.14;

    std::cout << std::get<double>(v);

    v = "abs";

    std::cout << std::get<std::string>(v)[1];
}
```

Если не угадать с типом, то будет исключение. А что если мы бы хотели вот следующую штуку.

```c++
std::variant<int, double, std::string> v = 'a';
// вот здесь будет по правилам перегрузки int
```

Если будет неоднозначность, то анвкука СЕ.

## std::visit

В визит мы передаем функтор, а также variant, причем функтор, должен принимать все параметры variant

Если что там переменное количество variant

```c++
std::visit([](const auto& item) -> int {
            if constexpr (std::is_same_v<decltype(item), const int&>) {
                std::cout << "int! " << item << '\n';
            } else {
                st::cout << "not int\n";
            }

            return 0;
        }, v);
```

Давайте рассмотрим теперь приминение к нескольким вариантам

```c++
std::visit([](const auto& item, const auto&) -> int {
            if constexpr (std::is_same_v<decltype(item), const int&>) {
                std::cout << "int! " << item << '\n';
            } else {
                st::cout << "not int\n";
            }

            return 0;
        }, v, v2);
```

Давайте рассмотрим теперь такой пример. Почему компилятор справляется с этим кодом? Каким образом if constexpr срабатывает с std::is_same? Тут шаблонный лямбда и когда мы пытаемся вызвать от вариант генеиться все виды этого оператора со всеми возможными типами который есть в вариант

```c++
std::vector<decltype(v)> vec;
vec.push_back(v);
vec.push_back(v2);

for (const auto& var: vec) {
    std::visit([](const auto& item, const auto&) -> int {
                if constexpr (std::is_same_v<decltype(item), const int&>) {
                    std::cout << "int! " << item << '\n';
                } else {
                    st::cout << "not int\n";
                }

                return 0;
            }, v, v2);
}
```

## Overload patterns

Вот у нас есть некоторый Overload, который наследуется от своих шаблонных переменных, а также круглые скобки, которые создаются для каждого переменных.

```c++
template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...; // since c++17
};

template<class... Ts> Overload(Ts...) -> Overload<Ts...>;
```

У нас есть вектор из вариантов

```c++
 std::vector<std::variant<std::vector<int>, double, std::string>>
        vecVariant = { 1.5, std::vector<int>{1, 2, 3, 4, 5}, "HELOO"};
```

А также есть display me, который принимает

```c++
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
```

А также вот эта штука, причем все компилируется, причем выводит все правильно.

```c++
    for (auto v : vecVariant) {
        std::visit(DisplayMe, v);
    }
```

Начиная с с++20 можно убрать следующую строчку.

```c++
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;
```
