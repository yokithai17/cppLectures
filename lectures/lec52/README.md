# Концепты (окончание). Лямбда-функции (начало)

## subsuming

Вот мы когда то писали std::advance, давайте перепишем изпользуя концепты.

Компилятор не умеет делать перегрузку по рекуарс, а здесь у нас разница по концептам. То вот если у нас разные концепта, то перегрузку можно было делать, **НО ТОЛЬКО ЕСЛИ
КОМПИЛЯТОР СПОСОБЕН ДОКАЗАТЬ**, что любой тип А может реализвоать В.

```c++
template <std::input_iterator Iter>
void advance(Iter& iter, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ++iter;
    }
}

template <std::random_access_iterator Iter>
void advance(Iter& iter, size_t n) {
    iter += n;
}
```

А как же компилятор может это доказать? Компилятор не умеет синтаксически разные requies, которые семантически одинаковые приводить к одному, НО он умеет если у нас атомарные requires делать, он как бы разворачивает requires, и если видит, что если у нас один частный другого то ДА.

## Partial ordering of contrainrs 

Давайте скажем что P subsume Q, если может быть доказано, что P imples Q

А теперь более формаьно. 

Давайте сконвентируем P в ДНФ а Q в КНФ и тогда будем считать, что P subsume Q, если каждый дизьюнкт P влечет коньюкь Q.

$ P = (A) \vee (B) \vee (C) $

$ Q = (D) \wedge (E) $

И нужно чтобы из Каждой A B C следовало D E. Это прям строгая темка.

```c++
template <typename T>
concept InputeIter = requires(T x) {
    *x;
    ++x;
};


template <typename T>
concept ForwardIter = InputeIter && requires(T x) {
    x++;
};


template <typename T>
concept BiderIter = ForwardIter && requires(T x) {
    --x;
    x--;
};

template <typename T>
concept RandomIter = ForwardIter && requires(T x, T y) {
    x - y;
    x < y; x > y; x <= y; x >= y;
    x -= 1; x += 1;
    x + 1; x - 1; 1 + x;
};
```

А вот если мы сделаем тупо копи паст каждого, то тогда subsuming не сработает. Если у нас один концеп частный другого то лучще лог и с этим концептом

Вот как раз для коммутативностьи нужна след темка

```c++
namespace detail {
    template <typename T, class U>
    concept SameHelper = std::is_same_v<T, U>;
}

template <typename T, typename U>
concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;
```

## std::equivalence_realtion

Оно типа ожидает, что должно выполняться, а по факту не проверяется

```c++
template <typename R, typename T, typename U>
concept equ_r = std::relation<R, T, U>;
```

Рекурсивные концепты нельзя...

Запрет делать require на концепт

## Basic usages of lamdas

```[](...){}``` Вот это некоторый экспрешен, можно было объявить лямбду заранее. И вот тип такой штуки это некоторый тип, который генирирует сам компилятор под вот эту лямбду. По сути  лямбда это функциональный объект, т.е тип у которой определеен оператор круглые скобки.

```c++
    std::vetor v = {1, 2, 3, 4};

    std::sort(v.begin(), v.end(),
        [](int x, int y){ return x * x < y * y;});
```

А вот, что если мы хотим сделать сет по компоратору.

```c++
auto cmp = [](int x, int y) { ...};
std::set<int, decltype(cmp)> s; // since c++20
std::set<int, decltype(cmp)> s(cmp); // until c++17
```

В изначально лямбда не дефолтно конструируется... Но он copy constract

Для каждой новой лямбды создается новая лямбда (новый класс) и синтакстические лямбды семантически разные

## lambda return type

Что насчет возвращаемого типа ?

По обычному выводу типов. Для лямбд автомотический тип, можно не указывать автомотический тип.

Как мне указать возвращаемый тип для лямбды.

```c++
[]() -> type {};
```
По дефолту возвращается по значению. Крч все как в обычном выводе типов для функциею.

```c++
auto inc = [](int& x) -> decltype(auto) {};
```

## Immeditae invocation

Еще полезная вещь в лямбде, когда мы хотим их как вызываемый объект в моменте;

```c++
[...](...){...}(); // rvalue
```
Если не передается аргументы можно пропускать их.

Вот будет ли он делать лишний jmp в ассемблере, вообще наверное да.

Зачем нужен Immediate invocation? Вот допустим у нас есть константная переменная

```c++
const int x = [](int y) {
    if (y % 2 == 0) {
        return y / 2;
    } else {
        return 3 * y + 1;
    }
}(12);
```

Теперь у нас константа инцилизируется разными способами.

Второй еще полезный пример, когда нужно в inizializer_list

```c++
S(..): x([](){}()) {}
```

