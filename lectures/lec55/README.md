# std::function и ее реализация. std::invoke

## std::function

```c++
int square(int x) {
  return x * x;
}

struct AddOne {
  int operator()(int x) {
    return x + 1;
  }
};

int main() {
  std::function<int(int)> f = square;

  f = AddOne();

  int c = 10;
  f = [&c](int x) { return x * 10; };

  auto f2 = f;
}
```

Причем все хорошо, насчет удаление полей при вызове деструкторе...

А все ли мы рассмотрели с коллабле? Нет, мы не рассмотрели передачу метода класса. Вот будет ли рабоать следующий код? Нет потому, что там еще неявно передается this, указаетель на объект S

```c++
struct Starnge {
  int moduloSeven(int n) {
    return n  % 7;
  }
};

std::functionla<int(int)> f = &Strange::moduloSeven;
```

Для этого нужен другой **std::functional**, в который будет передаваться уже ссылка на объект класса.

```c++
std::functional<int(Strange&, int)> ff = &Strange::moduleSeven;
```

Еще коллабле считается полем. Ну вообще это полезтно, можно рассматриваит эта как проектор.

```c++
struct S {
  int x;
};

std::functional<int(S&)> f = &S::x;
```