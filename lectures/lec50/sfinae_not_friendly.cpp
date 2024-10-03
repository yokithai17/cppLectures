#include <iostream>
#include <list>

template <typename T>
auto call_f(const T& t) { // not SFINAE-friendly
    t.f();
}

template <typename T>
auto callf_f_if_availvalbe_impl(const T& t, int) -> decltype(call_f) {
    call_f(t);
}


template <typename T>
auto callf_f_if_availvalbe_impl(const T& t, ...) -> decltype(call_f) {
    // DO NOTIng
}

