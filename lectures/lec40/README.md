# Ссылочные квалификаторы. Универсальные ссылки. Функция std::forward

## Способ как можно интуитивно понимать std::move

```meramaid
graph TD;
    type-->| | type&
    const_type-->const_type&
    type-->const_type
    const_type-->|const_cast|type&
```

```meramaid
graph TD;
    Base-->Base&
    Derived-->Base
    Derived-->Derived&
    Base-->|static_cast|Derived&
```

Ну что такое **const** это же тот же объект с урезанным функционалом.

```meramaid
graph TD;
    rvalue-->const_type&
    rvalue-->type&&
    lvalue-->const_type&
    lvalue-->|std::move|type&&
```

Тут по факту аналогично.

## Напоминание про rvalue-ссылок

```c++
int x = 5;
int&& y = std::move(x);
int&& z = y; //CE
int&& z = std::move(y);
int t = 6;
z = t; // x = 6
int&& r = 1; // lifetime expand
```

А что если добавим теперь константность? **Const** ортогональна

```c++
const int&& y = std::move(x); // OK
const int&& r = 1; // OK
int&& z = std::move(y); // CE 
```

Вспомним как у нас выглядил **std::move**

```c++
T&& move(T& x) {
    return static_cast<T&&>(x);
}
```

И где то мы сделали с константом?

```c++
void f(const T& x) {
    g(std::move(x)); // Будет создана копия
}
```

Когда мы передаем, что то в шаблонную функцию амперсанда отбрасывается

## reference qualifiers (ссылочные квалификаторы)

```c++
struct S {
    std::string str;
    std::string get_data() const {
        return str;
    }
}
```

А теперь пусть у нас S временный объект, произойдет копирование. **S{"abs"}.get_data()**. Здесь this rvalue reference

Вот для этого начиная с++11 Ref qual.

```c++
std::string get_data() && {
    return std::move(str);
}
```

Симмитричер этому есть методы, которые только есть только для lvalue

```c++
std::string get_data() &;
```

Заметим, что *const* у нас также работает. То есть если сделали ```type foo() const &;```, то у нас на этот метод можно передать rvalue.

Вспомним bigint, почему у нас **a + b = 5** работало. Для нормальной работы мы сделали **operator+() &*.

# 10.5 Forwarding reference and std::forward

## Почему rvalue-ссылок недостаточно для решения всех проблем

Почему с пуш беком у нас все еще не так.

```c++
void push_back(const T&);
void push_back(T&&);
```

И там написано аллокатор.констракт, которая принимает *const T&*.

Такая же проблема с функцией *emplace_back*.

Мы хотим передавать дальше в функциях с такими же value

```c++
template <typename Args>
void emplace_back(Args&&... args)
```

Вот такая вот сигнатура, и как раз таки **std::forward**, нужна для решение проблемы. Утверждается, что этот форвард сохраняет вид аргумента.

```c++
void construct(U* ptr, Args&&... args) {
    new (ptr) U(std::forward<Args>(args)...);
}
```

## Понятие универсальных ссылок (forwarding reference)

Что нас смущает с этой реализацие? Как будто, у нас rvalue, которое принимает lvalue, но как???

*ПРАВИЛО*
- Если у нас функция которая принимает *T* с 2 амперсандами, причем *T* это шаблон этой функции (не шаблон класса, а именно этой функции), то она имеет другое правило. ПРИКАЗ КОМИТЕТА ПО СТАНДАРТИЗАЦИИ.

Вот такие ссылки и называется универсальные ссылки или форвардинг ссылки, потому что они кидается в *std::forward*



## Упражнение: создание вектора из переменного числа аргумента.


```c++
struct S {
    vector<string> v;

    template <typename... Args>
    S(Args&&... args)  {
        static_assert(std::is_convertible(std::string, Args...)); // Все типы конвертируемы к string
        v.reserve(sizeof...(Args));
        v.push_back((std::forward<Args>), ...);
    }
};
```

## Правило вывода шаблонного типа для обычных и для универсальных ссылок

```c++
template <typename T>
void h(T&& x);

void g(T& x);

void f(T x);

int x = 0;
f(x); // T := int
g(x); // T := int
h(x); // T := int&
int& y = x;
f(y); // T := int
g(y); // T := int
h(y); // T := int&
int&& z = std::move(x);
f(z); // T := int
g(z); // T := int
h(z); // T := int&
```

Тут правило если мы вызываемя от rvalue -> T, а если от lvalue -> T&. Это для универсальных ссылок.

А какой тип имеет x? Вот для этого в языке есть *reference collapsing*.

```
& + & -> &
&& + & -> &
& + && -> &
&& + && -> &&
```