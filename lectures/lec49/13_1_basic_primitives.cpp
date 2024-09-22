#include <iostream>
#include <vector>

// XIII. Metafunctions. SFINAE concepts and constarints

template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

template <typename T, T x>
struct integral_constant {
    static constexpr T value = x;
};

template <bool b>
using bool_constant = integral_constant<bool, b>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename T>
struct is_lvalue_reference: false_type {};

template <typename T>
struct is_lvalue_reference<T&>: true_type {};

template <typename T>
using is_lvalue_reference_v = is_lvalue_reference<T>::value;

template<typename...> struct conjunction : std::true_type {};

template<typename B1> struct conjunction<B1> : B1 {};

template<typename B1, typename... Bn>
struct conjunction<B1, Bn...>
    : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

// 13.2. SFINAE and std::enable_if
// SFINAE = Substituion Failure Is Not An Error!

template <bool B>
struct enable_if {};

template <>
struct enable_if<true> {
    using type = void;
}

template <bool B>
using enable_if_t = enable_if<B>::type;


template <typename T,
          typename = enable_if< std::is_integral_v<T> >>
auto f(T) {
    std::cout << 1;
}

// C-style variadic function
void f(...) {
    std::cout << 2;
}

namespace detail {
  template <class T>
  std::true_type test(int T::*); // указатель на член класса типа int
                                 // который всегда есть в классе (при объявление)

  template <class>
  std::false_type test(...);
}


template <class T>
struct is_class: decltype(detail::test<T>(nullptr)) {};

// 13.3 Metafunction implementation with usage of SFINAE

namespace detail {
    template <typename T, typename... Args>
    std::true_type test( decltype(T().construct(Args()...))* );

    template <typename...>
    std::false_type test(...);
}

template <typename T, typename... Args>
struct has_method_construct: decltype(detail::test<T, Args...>(nullptr)) {};

template <typename T, typename... Args>
const bool has_method_construct_v = has_method_construct<T, Args...>::value;


namespace detail {
    template <typename T, typename... Args>
    std::true_type test_icc(decltype(T(std::declval<T&>(), nullptr));

    template <typename...>
    std::false_type test_icc(...);
}

template <typename T, typename... Args>
struct is_copy_constructible: decltype(detail::test_icc<T>(nullptr)) {};

namespace detail {
    template <typename T, typename... Args>
    std::true_type test_intmc(decltype(T(std::declval<T&>(), nullptr)));

    template <typename...>
    std::false_type test_intmc(...);
}

template <typename T, typename... Args>
struct is_nothrow_move_constructible: decltype(detail::test_intmc<T>(nullptr)) {};

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

int main() {
  //f(0);
  std::vector<int> v;
  //f(v);
}