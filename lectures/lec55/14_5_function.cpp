#include <iostream>
#include <functional>

// 14.5. std::function usage and naive implementation

int square(int x) {
  return x * x;
}

struct AddOne {
  int operator()(int x) {
    return x + 1;
  }
};

struct Starnge {
  int moduloSeven(int n) {
    return n  % 7;
  }
};

int main() {
  std::function<int(int)> f = square;

  f = AddOne();

  int c = 10;
  f = [&c](int x) { return x * 10; };

  auto f2 = f;
}