# Лямбда-функции. Списки захвата. Замыкания

## Captures in lambdas

Вот когда мы лямбде мы не видим локальные переменные, а вот для того, чтобы мы имелеи доспут нужно их захваитиь

И это делается для того, чтобы захватить нужно передать в квадратные скобки

```c++
    std::vector v = {1, 2, 3, 4, 5};

    int c = 3;

    auto cmp = [с](int x, int y) {
        return (x - c) * (x - c) 
        < (y - c) * (y - c);
    };

    std::sort(v.begin(), v.end(), cmp);
```

Вопрос где хранится эта переменная? ... Чуть позже

Правда ли, что sub скопируется? Потому, то что мы передаем копируется в поле лямбды (там рельано класс создается смотри cpp_insight), ну вот так как нам сделать по указателю

```c++
    std::vector<std::string> v = {"dsa", "bavae", "fefeaa"};

    std::string sub = "av";

    auto con_sub = [sub -> &sub](const std::string& str) {
        return str.find(sub) != std::string::npos;
    };

    auto iter = std:find_if(v.begin(), v.end(),
    con_sub);

    std::cout << *iter;
```

А сохранили ли мы константную ссылку или нет? Мы захватили по не константной ссылке.

А что если мы захватили по значению, и потом меняем? Нет мы так не можем... Мы хотим, чтобы оператор круглые скобки был константный метод....

Что же происходит???? Почему же то, что мы захватили по ссылке можем менять, а по значению не можем...

Как сделать, чтобы лямбда могла менять захваченные переменные ? **Нужно Mutable**

```c++
auto con_sub = [sub](const std::string& str) mutable {
    return str.find(sub) != std::string::npos;
};
```

А давайте попрубуыем с Mutable сделать set. Вот такое скомпилируется? Нет так как сет ожидает константные ключи

```c++
auto cmp = [r](int& x, int& y) mutable { return x < y; };
std::set<int, decltype(cmp)> st;
```

## Capture with intializion

```c++
[sub = sub + 'g'](const auto& str {};
```
Но этот синтаксис дает некоторый побочный, к примеру **std::move**.

Но все же как захватить по константной ссылке?

```c++
[&val = std::as_const(val)](){}
```

А что будет если мы напишем следующий код? Ошибка компиляцие, потому что мы lvalue через rvalue хотим проинзолицировать.

```c++
[&val = std::move(val)](){}
```

Несколько переменных в захвате, просто через запятую. А чтобы захватить все по значению мы можем сделать вот след штуку. Чтобы все по ссылке, **[&]**

```c++
[=](){};
```

Это появилось в с++11, но в с++14 комитет понял, что эта шняга, и попросил не пользоваться этим.

## Capture with packets

Вот здесь они по ссылке.

```c++
template <typename... Strings>
requires ((std::is_same_v<Strings, std::string> && ...))
    void test(const Strings&... subs) {
    auto cn_s = [&subs...](const std::string& str) {
        return ((str.find(subs) != std::string::npos) && ...);
    };
}
```

А если мы хотим с инцилизацией, к примеру, чтобы мувнуть.

```c++
template <typename... Strings>
requires ((std::is_same_v<Strings, std::string> && ...))
    void test(const Strings&... subs) {
    auto cn_s = [...subs = std::move(subs)](const std::string& str) {
        return ((str.find(subs) != std::string::npos) && ...);
    };
}
```

## Capture this

Вот как будто все хорошо, даже в этом коде как и предсказывалсь напиешстя 8.

```c++
struct S {
    int a = 3;

    auto getLambda() {
        auto f = [=](int x) {
            return x + a;
        };
        return f;
    }
};

int main() {
    S s;
    auto f = s.getLambda();

    std::cout << f(5);
}
```

А если мы с вами перепишем код. То это будет UB, потому что = захваетывает локальные переменные, но для структры a не локальная перменная, а на самом деле мы не а захватили объекта в котором мы находимся, так сказать мы захватили this

```c++
int main() {
    S* s = new S;
    auto f = s->getLambda();
    delete s;

    std::cout << f(5);
}
```

Вот к примеру как это было написано, если мы хотим по значению **[a = a]**

```c++
    auto getLambda() {
        auto f = [this](int x) {
            return x + a; // this неявно в каститься в this->a
        };
        return f;
    }
```

Самая большая проблема в лямбде, что она не должна пережить ребят, которых пережила.

Что же будет в следующем коде? Если у нас статические переменные, то они не захватываются, потому что считается глобальные. Т.е захват ничего не сделает.

```c++
    static int x = 0;

    auto f = [=]() mutable {
        ++x;
    };

    f();

    std::cout << x;
```

ЗАбавный пример. Что выведется. У нас есть func1 and func2, это объекты одно и того же типа, которые созданы


```c++
auto factory (int parametr) {
  static int a = 0;              // static fo factory создается 1 раз, в лямбду не захватывается
  return [=] (int argument) {
    static int b = 0;            // Local static when init этот метод
    a += parametr; b += argument;
    return a + b;
  };
}


int main() {

  auto func1 = factory(1);
  auto func2 = factory(2);
  static_assert(std::is_same_v<decltype(func1), decltype(func2)>); // OK

  std::cout << func1(20) << " " << func1(30) << " "
    << func2(20) << " " << func2(30) << std::endl; // 21 52 74 106
}
```

