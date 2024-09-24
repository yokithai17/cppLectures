# SFINAE-friendliness. Constraints and requirements

## std::is_convertible

```c++
namespace detail
{
    template<class T>
    auto test_returnable(int) -> decltype(
        void(static_cast<T(*)()>(nullptr)), std::true_type{}
    );
    template<class>
    auto test_returnable(...) -> std::false_type;

    template<class From, class To>
    auto test_implicitly_convertible(int) -> decltype(
        void(std::declval<void(&)(To)>()(std::declval<From>())), std::true_type{}
    );
    template<class, class>
    auto test_implicitly_convertible(...) -> std::false_type;
} // namespace detail

template<class From, class To>
struct is_convertible : std::integral_constant<bool,
    (decltype(detail::test_returnable<To>(0))::value &&
     decltype(detail::test_implicitly_convertible<From, To>(0))::value) ||
    (std::is_void<From>::value && std::is_void<To>::value)
> {};
```

Почему вокруг этой функцие написан void, честно говоря хз...

## std::comman_type

Возвращает что то типа LCA для типов, если общего родителя нет среди данных, то он не найдет, если есть сын и дочь, то мамы мы не найдем, это будет СЕ.

А что если было сын дочь мама, то зависит от порядка... И стандартный common_type так и был реализован до с++17, потом реализация поменялась. А чем же плох был? Он не был SFINAE-friendly

```c++
// common_type

template <typename... Types>
struct common_type;

template <typename... T>
struct common_type<T>: std::type_identity<T> {};

template <typename T, typename U>
struct common_type<T, U>
    : std::type_identity<
    decltype(true ? std::declval<T>() : std::declval<U>())
    > {};

template <typename T, typename... Types>
struct common_type<T, Types...>
    : common_type<T, common_type<...>::type> {};
```


## SFINAE-friendly

Вот заметим, что следующася функция не SFINAe френдли, но почему же? Будеи ошибка, что у t нет функцие f()..., потому что глядя на определение он не понимает, что нужно возвращать... потому что инстацирует тело, чтоьы понять.

Как можем поменять, сделаем вот так ```auto -> decltype(t.f(), void())```. и теперь эта функция будет SFINAE френдли.

```c++
template <typename T>
auto call_f(const T& t) { // not SFINAE-friendly
    t.f();
}

template <typename T>
auto callf_f_if_availvalbe_impl(const T& t, int) -> decltype(call_f) {
    call_f(t);
}
```

## consraints and requirements (since c++20)

В с++20 наконец-то дали доступ к нормальному требованию от типа. Мы можем еще писать перед сигнатурой, или после скобки, должно все быть компайл тайм вычислимое, это кс-та все були, так что можно через логические операцие делать.

requires можно писать только один раз, а потом bool-expression

```c++
template <typename T>
requires std::is_class_v<T>
    void f(const T&) {
}
```


а что елси мы хотим делать перегрузку? requires проверяется на стадие поиска имен, с точки зрения комплятора, что можно выбрать перегрзуку, так Вот до интсацирование и выбора перегрузки requirements выбирается раньше.

Учитывется, что наличие рек лучше чем отсутствие для перегрузки. (если 1 с рек, а у другой нет)

Так вот если у нас 2 разных requires, то у нас хоть и одинаковые функции компилятор считает это как разные названия. (если 2 с рек то СЕ)

Короче говоря перегрузка не может понять, что строже

Нельзя вещать requires на не шаблонные функции, так как были бы проблемы на стадие линковки.

можно к прмеру на класс навещать.

```c++
template <typename T, size_t N>
requires (N <= 1000)
struct array {
    T arr[N];
};
```
