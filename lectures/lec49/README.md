# Реализация type_traits с помощью SFINAE

## Проверка наличия методов в классе. Comma trick

Для чего же здесь оператор запятая: мы хотим, чтобы у нас был вминяемый тип, и мы под decltype загоняем 2 выражение разделенной запятой, тип нашего выражения всегда nullptr_t, а вот является ли оно корректным от 1 части запятой.

```c++
namespace detail {
    //comma trick
    template <typename T, typename... Args>
    std::true_type test(decltype(T().construct(Args()...), nullptr));

    template <typename...>
    std::false_type test(...);
}

template <typename T, typename... Args>
struct has_method_construct: decltype(detail::test<T, Args...>(nullptr)) {};

template <typename T, typename... Args>
const bool has_method_construct_v = has_method_construct<T, Args...>::value;
```

## declval

Проблема, может не быть конструктора по умолчанию к примеру мы можем сделать следующие

```(*static_cast<T*>(nullptr)).construct(*static_cast<Args*>(nullptr)...)``` 

Но это не очень, а вот цивилизованный вариант это изпользовать declval. А как она реализована, НИКАК у нет реализации. Она только для того, чтобы вызываться под compile time темками.

```c++
template <typename T>
T declval();
```

Однако и в этой реализации есть проблема, к примеру, что если мы вызваемся от incomplete типа, то CE, а вот уже следующая реализация дает??? Вот если конструктор принимает по значению то компилятор хочет проверить, что тип хотя бы complete, однако если мы через &&, то файн. МЕГА ГРОБ.

```c++
template <typename T>
T&& declval() noexcept;
```

## std::is_copy_constructible

По факту нужно сделать, тоже самое, что мы и сделали в has_method, только вызвать другие темки

```c++
namespace detail {
    template <typename T, typename... Args>
    std::true_type test_icc(decltype(T(std::declval<T&>(), nullptr));

    template <typename...>
    std::false_type test_icc(...);
}

template <typename T, typename... Args>
struct is_copy_constructible: decltype(detail::test_icc<T>(nullptr)) {};
```

## is_based_of

```c++
namespace detail {

    template <typename B, typename D>
    std::true_type test_if_base_of(B*);

    template <typename...>
    std::false_type test_if_base_of(...);
}

template <typename B, typename D>
struct is_base_of : std::conjunction<
                        std::is_class<B>,
                        std::is_class<D>,
                        decltype(detail::test_if_base_of<B, D>(static_cast<D*>(nullptr)))
                    > {};
```

Однако такая реализация может кидать CE, так как у нас есть проверка приватности, потому что бывает приватное наследование.

Нужно просто еще сделать одну обертку над этой штукой.

