# SFINAE, std::enable_if


## Базовые примитивы

Вспомним некторые примитивы, которые мы изучали в 1 семестре.

```c++
template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
struct type_identity_t = type_identity<T>::type;
```

```c++
template <typename T, T x>
struct integral_constant {
    static constexpr T value = x;
};

template <bool b>
using bool_constant = integral_constant<bool, b>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;
```

## is_lvalue_reference

Давайте теперь для примера реализуем проверку для Lvalue reference 

```c++
template <typename T>
struct is_lvalue_reference: false_type {};

template <typename T>
struct is_lvalue_reference<T&>: true_type {};

template <typename T>
struct is_lvalue_reference_v = using is_lvalue_reference<T>::value;
```

## conjunctuion

```c++
template<typename...> struct conjunction : std::true_type {};

template<typename B1> struct conjunction<B1> : B1 {};

template<typename B1, typename... Bn>
struct conjunction<B1, Bn...>
    : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};
```


## SFINAE 

когда комплятор выбирает какую функцию выбрать в перегрузку... В момент когда компилятор подставляет в объявление функции шаблонные тип и получается некорректный тип, то это не ошибка компиляции, просто компилятор выбрасывает эту перегрузку.

Давайте рассмотрим пример

```c++
template <typename T>
auto f(T) -> typename T::value_type {
    std::cout << 1;
}

// C-style variadic function
void f(...) {
    std::cout << 2;
}

f(std::vector<int>()); // 1
f(0); // 2
```

### ТОЛЬКО В ОБЪЯВЛЕНИЕ ФУНКЦИИ РАБОТАЕТ SFINAE

Если уже в теле происходит ошибка, то это ошибка компиляции. 

## std::enable_if

```c++
template <bool B>
struct enable_if {};

template <>
struct enable_if<true> {
    using type = void;
}

template <bool B>
using enable_if_t = enable_if<B>::type;
```


```c++
template <typename T,
          typename = enable_if< std::is_integral_v<T> >>
auto f(T) {
    std::cout << 1;
}

// C-style variadic function
void f(...) {
    std::cout << 2;
}

f(vector); // 2
f();       // 1 
```

Но у нас сейчас случай, когда в 1м 1 параметр, а во втором много параметров. Как нам добитьбся, чтобы мы могли работать с 1 параметром ин есколько перегрузок

Отрицание enable_if не поможет

К примеру можно сделать typename = void Добавить.

```c++
template <typename T,
          typename = enable_if< std::is_integral_v<T> >>
auto f(T) {
    std::cout << 1;
}

template <typename T,
          typename = enable_if<!std::is_integral_v<T>>,
          typename = void>
auto f(T) {
    std::cout << 2;
}
```

Однако это не каноничный вариант, а вот сейчас каноничный

```c++
template <typename T, enable_if<std::is_integral_v<T>, bool> = true>
auto f(T) {
    std::cout << 1;
}

template <typename T, enable_if<!std::is_integral_v<T>, bool> = true>
auto f(T) {
    std::cout << 2;
}
```

Вот еnable_if либо невалидный тип либо bool, причем если мы поменяем true на false где-то ничешл не поменяется.

С точки зрения шаблонной темки ```enable_if<std::is_integral_v<T>, bool>``` И ```enable_if<!std::is_integral_v<T>, bool>``` разные вещи и не считает это redefinition


А что если мы будем передавать пакет (он вычисляется жадно), то там как-то странно 

## std::is_class

Как проверить является ли что классом или структурой

```c++
namespace detail {
  template <class T>
  std::true_type test(int T::*); // указатель на член класса типа int
                                 // который всегда есть в классе (при объявление)

  template <class>
  std::false_type test(...);
}


template <class T>
struct is_class: decltype(detail::test<T>(nullptr)) {};
```

## std::is_polymorphic

Если в классе есть хоть одна виртуальная функция, то возвращает true


```c++
namespace detail {
    template <class T>
    std::true_type detect_is_polymorphic(
        decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr)))
    );
    template <class T>
    std::false_type detect_is_polymorphic(...);
}

template <class T>
struct is_polymorphic : decltype(detail::detec_is_polymorphic<T>(nullptr)) {};
```

Если мы хотим динамик каст сделать, когда у нас не полиморфный тип, то тогда у нас 
не будет динамик каста (не CE так как мы под decltype), а тогда мы выберем std::false_type

