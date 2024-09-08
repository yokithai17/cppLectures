# Вывод типов auto, decltype, decltype(auto)


## ключевое слово auto

У нас уже есть механизмы отгадывания типов, к примеру те же шаблоны, проблема будет с разными ссылками

У expression а есть уже темки. заметим, что у auto уже будет ссылки вшиты

Как работает ```auto&&``` это работает как и у шаблоннах, причем здесь lvalue referecnce. Т.е работает как универсальная ссылка

Вот следующий блок кода лучше, чем просто *auto&*, по понятным из прошлых лекциях причинам.

```c++
template <typename Container>
void f(Container&& container) {
    for (auto&& value: std::forward<Container>(container)) {
        // ...
    }
}
```

## комбинайи auto с ссылками и пойнтерами

```c++
    auto x = 5;
    auto& y = x;
    const auto& z = y;
    auto&& t = x;
    auto& a = 0; // CE
    auto&& b = 0; // OK life time 
    std::vector<auto> v = {1, 2}; // CE
    int* p = new auto(5);
```

## auto возвращаемый тип из функции

```c++
template <typename T>
auto g(T x) {
    return ++x;
}

auto x =  g(5); // int
auto& y = g(5); // CE так как справо rvalue
```

ЭТО UB так как нам вернется битая ссылка, на стадие выхода 

```c++
template <typename T>
auto& g(T x) {
    return ++x;
}
```

Вот теперь такой пример ? Что будет? Ошибкак компиляции

```c++
template <typename T>
auto g(T x) {
    if (x > 0)
        return 0;
    else
        return 1u;
}
```

А вот такой пример? Да он компилирует, причем *if constexpr* раньше парсится чем *auto*

```c++
template <typename T>
auto g() {
    if constexpr (std::is_same_v<T, int>)
        return 0;
    else
        return 1u;
}
```

А что если мы сделаем вот так ? Компилятор не упадет в бесонечную рекурсию. Есть защита.

```c++
template <typename T>
auto g(T x) {
    if constexpr (std::is_same_v<T, int>)
        return g(x);
    else
        return 1u;
}
```

## синтаксис trailing return type

```c++
template <typename T>
auto move(T&& value) -> std::remove_reference_t<T>&&
{
    return static_cast<std::remove_reference_t<T>&&>(value);
}
```

## auto в принимаемых since c++20

По факту это тоже самое что и шаблонны

```c++
void f(auto x) {}
```

Кста можно еще в параметрах шабонна

## Особенность auto при использование initializer_list

```c++
auto lst = {1, 2, 3}; // OK
h({1, 2, 3}); // CE h(auto);
```

## decltype от переменных

про произвольный экспрешен можно узнать какой у него тип
и в компайл тайме узнать этот тип

```c++
int x = 0;
declptype(x) v = x;
```

**В отличие от auto он не отбрасывает ссылки, ОН ПОКАЗЫВАЕТ ТОЧНЫЙ ТИП**

На него можно навесить приколы

Как понять какой тип *decltype* выводит

```c++
template <typename T>
struct Debug {
    Debug(T) = delete;
}
```

Если что референс колапсинг также работает как и всегда

Заметим что мы можем брать от любого выражени

Вот к примеру такое выражение что выведется ? 11, почему потому *decltype* не вычисляет выражение, а лишь смотрит у него тип

```c++
int main() {
    int x = 0;
    decltype(++x) u = x;
    ++u;

    std::cout << x << u;
}
```

```c++
decltype(throw 1)* p = &x; // OK void* 
```

Вот еще один интересный пример. Что будет? int&... Потому что decltype работает по разному от имени переменной и от expressions

```c++
int x = 0;
decltype((x)) y = x;
```

1) if the value category of expression is xvalue, then decltype yields T&&;
2) if the value category of expression is lvalue, then decltype yields T&;
3) if the value category of expression is prvalue, then decltype yields T.


## decltype в возвращаемом типе функция

Казалось бы следующий блок кода работает как нужно, но на самом деле это будет не всегда работать. К примеру vector<bool>

```c++
template <typename Conatiner>
auto& getElem(Container& cont, size_t index) {
    return cont[index];
}
```

Поэтому можно 

```c++
template <typename Conatiner>
auto& getElem(Container& cont, size_t index)
    -> decltype(cont[index]) 
{
    return cont[index];
}
```

## Упрощенние реализации emplace_back с помощью decltype

```c++
void emplace_back(auto&&... args) {
    if (sz == cap) {
        reserve(...)
    }
    ac::construct(alloc, arr + sz,
        std::forward<decltype(args)>(args)...);
} 
```