## Deducing this. CTAD. Structured bindings, tuples

## Deducing this

Был такой класс std::optinal, который появился в c++17


И там опрератор звездочка и опретор стрелочку, и правильная реализация состоит в 4 разных перегрузках

```c++
T& operator*() & {...}
T&& operator*() && {...}
const T& operator*() const& {...}
const T&& operator*() const && {...}
```

А также метод value, который также 4 должен иметь перегрузку а также ждя ....

И комитет по стандартизации сказал ХВАТИТ ЭТО ТЕРПЕТЬ и появилось **Deducing this* since c++23. Как в питоне явное принятие self.

Тут еще **Declval** но типо мы это не знаем.

```c++
template <typename Self>
decltype(auto) value(this Self&& self) {
    if (!init) {
        throw std::bad_optional_acces();
    }
    using DesiredType = decltype(std::forward_like<decltype(self)>(std::declval<T>()));
    return reinterpre_cast<DesiredType>(*self.value);
}

template <typename Self>
decltype(auto) value(this auto&& self);

template <typename T>
decltype(auto) foo(this auto self); // По значению
```

## std::forward_like 

она тоже в с++23.

Она как форвард, только сохраняет тип, но другую темкку. Т.Е она сохраняет свойства над типов и подменяет типы

```c++
template<class T, class U>
constexpr auto&& forward_like(U&& x) noexcept
{
    constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
    if constexpr (std::is_lvalue_reference_v<T&&>)
    {
        if constexpr (is_adding_const)
            return std::as_const(x);
        else
            return static_cast<U&>(x);
    }
    else
    {
        if constexpr (is_adding_const)
            return std::move(std::as_const(x));
        else
            return std::move(x);
    }
}
```

## CTAD

Начиная с с++17 можно писать след штуку

```c++
std::vector v = {1, 2, 3, 4, 5};
```

Вот что будет в следующем примемере? Будет ввектор из итераторов, потому что мы вызвались от инишалазер листа, у которого приоритет выше.

```c++
std::vector v = {1, 2, 3, 4};

std::vector v2{v.begin(), v.end()};
```

А если сделать круглые? Векор интов... Но каким же образом он смог догадаться, что у нас конструктор от итераторов. И вот оказывается у нас есть интсрукции каким образом догадываться.

```c++
std::vector v = {1, 2, 3, 4};

std::vector v2(v.begin(), v.end());
```

## Template Deduction Guide

Т.е вы можете в своем классе явно прописать каким образом будет выводиться тип **Template Deduction guide**

```c++
template <typename T>
class vector {
    template <typename Iter, Iter>
    vector(Iter, Iter) {}
};

template <typename Iter>
vector(Iter, Iter)
    -> vector<typename std::iterator_trairs<Iter>::value_type>;
```

А вот в с++20 еще и для Alias появились...

## Пример с агрегатной инцилизацией

Справиться ли CTAD со следующим примером? Да, но только начиная с с++20.

```c++
template <typenmae T, typename U>
struct S {
    T x;
    U y;
};

int main() {
    S s{1, 2.0}
}
```

А вот с таким примером? ДА но также начиная с с++20, но в с++17 только если написать дедакшин гайд.

```c++
struct A {};
struct B {};

S : T, U {};

S s{A(), B()};
```

А теперь супер упоротый пример? Да ткакже в с++20

```c++
template <typename... Args>
struct S: Args... {};

S s{A(), B()};
```

## std::tuple

Как мы уже знаем ое умеем дедактить

```c++
int main() {
    std::tuple t{1, 2.0, 'a'};
}
```

И чтобы мы хотели делать? ну к примеру получать по индекцу элемент

```c++
auto& d = std::get<1>(t);
```

И тут интересно какой тип возвращается ? ... 

Конечно очевидно, что мы можем расматривать универсальную ссылку.

```c++
auto&& d = std::get<1>(t);
auto&& d = std::get<1>(std::move(t));
```

## Structutred bindings (since c++17)

А что если мы хотим распоковать 


```c++
auto [x, y, z] = t; // copy
auto& [x, y, z] = t; // lvalue reference
auto&& [x, y, z] = t; // Универсальная ссылка
```

Такое не только к типу можно приминиь, а к примеру к паре

А как это работает? Ну там смотри сpp reference...

1) array
2) tuple-like type
    Это такой тип, для которого коректна определенная мета функция std::tuple_element<i, E>::type AND std::tuple_size<E>::size
3) struct // ... YASNO

## std::make_tuple

Вот примерно он так и реализован?.. 

```c++
template <typename... Types>
auto make_tuple(Types&&... args)
        -> tuple<std::decay_t<Types>...> {
    return {std::forward<std::decay_t<Types>>(args)...};
}
```

Вот давайте поймем в чем проблема в следующем примере? В конструкторе создался временный тюпл из копий, а проинцилизировали тюплами из ссылок. Тье мы получили сылки на временный объекты.

```c++
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
```

## std::tie

ЛЯ решения есть std::tie, котора создает тупл от л валью реф 

```c++
template <typename... Args>
constexpr
std::tuple<Args&...> ti(Args&... args) noexcept
{
    return {args...};
}
```

Соответсвенно 

```c++
    explicit X(const Ts&... values)
        : values(std::tie(values...)) {}
```

## std::forward_as_tuple

как std::make_tuple только без декея

## Некоторые примеры

```c++
std::set<S>::iterator iter;
bool is_ins;

std::tie(iter, is_ins) = set_of_s.insert(value);
```

## std::ignore_t

И вот такая штука

```c++
struct ignore_t {
    template <typename U>
    void operator=(const U&) {}
}

ignore_t ignore;
```

Вот к примеру изрользование.

```c++
std::tie(iter, std::ignore) = set_of.insert(v);
```

## piecewise_construct_t

Это нужно когда мы хотим прямо на месте создать пару..? не понятно. 

Вот такая штука есть у пары

```c++
template< class... Args1, class... Args2 >
pair( std::piecewise_construct_t,
      std::tuple<Args1...> first_args,
      std::tuple<Args2...> second_args );
```

Вот пример Изпользования

```c++
std::map<std::string, std::strin> m;
m.emplace(std::picewise_construct,
        std::forward_as_tuple("C"),
        std::forward_as_tuple(10, 'c');
)

// an alternative is m.try_emplace("c", 10, 'c');
```


## naive std::tuple implements (1 приближение)

Давайте идейно придумаем? Вот один из вариантов хранение.

```c++
template <typename Head, typename... Tail>
struct tuple {
    Head head;
    tuple<Tail...> tail;
};
```

А можно еще вот так сделать.

```c++
template <typename Head, typename... Tail>
struct tuple : tuple<Tail..> {
    Head head;
};
```

Будем рассматривать 1 вариант

```c++
template <typename... Types>
struct tuple;

template <typename Head, typename... Tail>
struct tuple {
    Head head;
    tuple<Tail...> tail;
};

template <>
struct tuple {};
```

А почему метод get внешняя функция? Потому что еслм она была частью класса то каждый раз когда наам нудно было ее вызывать от шаблоного тупла пришлось бы пистать ```t.templaet get<1>()```

Давайте напишем реализацию 1 из 4 get ов 

```c++
template <size_t N, typename... Types>
decltype(auto) get(tuple<Types...>& t) {
    if constexpr (N == 0) {
        return t.head;
    } else {
        return get<N - 1>(t.tail);
    }
}
```

Получается get друг тупла

```c++
template <size_t N, typenmae... Types>
friend decltype(auto) get(tuple<Types...>&);
```

А можем ли мы объеденить в одну?? Пока мы не знаем, так как у нас нет универсальной ссылки, которая будет принимать только один тип

## tuple_cat

Конкатенация многих tuple, мы могли бы сделать тупл кэт от 1 + тэйл ==> Осталось понять как сделать tuple_cat от 2 

Поймем как мы сделаем от 2, можно создать тэг, чтобы поподать в приватный конструктор.

Однако сейчас у нас пока работает тупл тег, создается много туплов.

```c++
struct tuple_cat_tag {};

template <typename... Ts, typename... Us>
auto tuple_cat(tuple<Ts...>& t, tuple<Us...>& u) {
    return tuple(tuple_cat_tag(), t.head, tuple_cat(t.tail, u));
}

...
```


## std::enable_if

expilisit( expression ) есть

Тут какой-то гроб....

У тупла есть конструктор такой что не должна существовать темка если существует i такой что он explicit от инишалайзер листа поэтому мы бы хотели его выключить

И вот для этого у нас есть enable_if

**possible implements**

```c++
template<bool B, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { typedef T type; };
```

Вот пример на изпользование **std::enable_if**

```c++
struct T {
    enum {int_t, float_t } type;

    template <typename Integer,
                std::enable_if_t<std::is_itegral<Integer>::value, bool> = true>
    T(Integer)
}
```


