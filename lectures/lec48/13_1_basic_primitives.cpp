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

int main() {
  //f(0);
  std::vector<int> v;
  f(v);
}