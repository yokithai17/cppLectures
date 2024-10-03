#include <iostream>

auto factory (int parametr) {
  static int a = 0;
  return [=] (int argument) {
    static int b = 0;
    a += parametr; b += argument;
    return a + b;
  };
}


int main() {

  auto func1 = factory(1);
  auto func2 = factory(2);

  std::cout << func1(20) << " " << func1(30) << " "
    << func2(20) << " " << func2(30) << std::endl;
}