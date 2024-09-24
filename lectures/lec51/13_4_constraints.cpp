#include <iostream>

template <typename T>
requires requires(T a, T b) { a + b; }
void test(const T& a, const T& b) {
  return a + b;
}

template <typename T>
requires requires(T a) {
  typename std::hash<T>; // ВСЕГДА БУДЕТ
  std::hash<T>()(a);     // А вот так уже нет
} void foo() {}

struct S {
  bool operator==(const S&) & = delete;
  bool operator==(const S&) && { return true; }
};

template<typename T>
requires(T a, T b) {
  std::move(a) == b;
};

int main() {}