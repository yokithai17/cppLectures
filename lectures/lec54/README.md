# Объекты замыканий. Обобщенные лямбды. Рекурсивные лямбды.

## Closures as objects

```c++
int main() {
  auto f = [](int x, int y){return x < y;};
}
```

И что же делает компилятор? Генерит некоторый класс. Заметим, что лямбду можно неявно привесту к такой же по сигнатуре как и лямбда.

```c++
int main()
{

  class __lambda_6_12
  {
    public:
    inline /*constexpr */ bool operator()(int x, int y) const
    {
      return x < y;
    }

    using retType_6_12 = bool (*)(int, int);
    inline constexpr operator retType_6_12 () const noexcept
    {
      return __invoke;
    };

    private:
    static inline /*constexpr */ bool __invoke(int x, int y)
    {
      return __lambda_6_12{}.operator()(x, y);
    }


  };

  __lambda_6_12 f = __lambda_6_12{};
  return 0;
}
```

Хорошо, а давайте сейчас мы с вами захватим некоторый объект. Захватим **int c = 0**.


```c++
int main()
{
  int c = 0;

  class __lambda_7_12
  {
    public:
    inline /*constexpr */ bool operator()(int x, int y) const // if mutable const disapear
    {
      return x < y;
    }

    private:
    int c;

    public:
    __lambda_7_12(int & _c)
    : c{_c}
    {}

  };

  __lambda_7_12 f = __lambda_7_12{c};
  return 0;
}
```

Кстати, если мы будем захватывать все переменные, однако по факту захватсяться (будут лежать в классе лямбда) только те переменные, которые мы изпользоватли.

А Вот присваивать запрещено, если мы что-то захватили.


## positive lambda hack

Вот что плюс означает перед лямбдой? это будет сишный пойнтер на функцию

```c++
auto f = +[](int x, int y) { return x < y; };
```

Вот, что будет у нас на cpp insight

```c++
  using FuncPtr_6 = bool (*)(int, int);
  FuncPtr_6 f = +__lambda_6_11{}.operator __lambda_6_11::retType_6_11();
```

## Geniralize (templated) labdas.

как вот мы хоти шаблонизировать лямбду? Мы можем просто шаблонизировать оператор круглые скобки.

```c++
int main() {
  std::map<std::string, int> m;
  m["abs"] = 1;

  decltype(m) m2;

  std::transform(m.begin(), m.end(),
                 std::inserter(m2, m2.begin()),
                 [](auto& item) {
                   ++item.second;
                   return item;
                   }
                );
}
```

А что если мы хотим шаблонны параметр явно написать, а не auto, к примеру навесить концепт

```c++
  auto f = [](auto item)
            requires requires(decltype(item) a) { a.seond; }
      {
          ++item.second;
          return item;
      };
```

Мы смогли навесить, но на самом деле это не красиво, а вот теперь по человечески

```c++
  auto f = []<typename T>(T& item)
            requires requires(T a) { a.seond; }
      {
          ++item.second;
          return item;
      };
```

## Functional programming

тут не тривиальное, что нужно из лямбды вернуть лямбду.

```c++
  auto sum = [](int x, int y) { return x + y; };

  // fix_first_argument
  auto fix_second_argument = [](auto&& f, auto&& second_arg) {
    return [f = std::forward<decltype(f)>(f)
            , second_arg = std::forward<decltype(second_arg)>(second_arg)]
          (auto&& first_arg) {
      return f(std::forward<decltype(first_arg)>(first_arg), second_arg);
    };
  };

  auto sum_with_five = fix_second_argument(sum, 5);

  // Currying (Карирование)
  sum_with_five(3); // must be 8
```


Давайте рассмотрим функцию, которая принимает функцию, которая делает еще давжды.

```c++
  auto print_hello = []() {
    std::cout << "hello";
  };

  auto do_twice = [](auto&& f) {
    return [f = std::forward<decltype(f)>(f)] (const auto&...args) {
        f(args...);
        f(args...);
    };

    do_twice(print_hello);
```

## Recursive lambda

```c++
  // Recursive lambda (since c++23)

    auto fiboncci = [](this auto& self, int n) -> int {
      return n > 2 ? self(n - 1) + self(n - 1) : 1;
    };
```


