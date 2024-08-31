# Функция std::move. Определение lvalue и rvalue. Rvalue-ссылки

Бывают типы, который мувать можно, а копировать нельзя. К примеру *unique_ptr*

Как компилятор понимает, когда ему нужно вызвать move конструктор? По тому lvalue или rvalue он принимает.

## 10.2 Mystic function std::move

*std::move* это штука, которая заставляет вызвать *move* операции, даже если у нас *lvalue*

```
struct S {
string data;
S(const string& data) : data(data) {} // lvalue obj
};

S(string&& data) : data(std::move(data)) {} // rvalue obj
```

Здесь все равно будет копирование, так как написано const
``` S(const string& data) : data(std::move(data))```

Как теперь решится проблема для *push_back*? Просто теперь нам нужна версия для *T&&*.

А разница будет лишь в одной строчке
```
new (ptr) T(val) -> new (ptr) T(std::move(ptr));
```

А что будет с исключениями?
```
string s;
throw s; <- Вот здесь будет move, тк он заведомо умрет
```

## Правильная реализация swap через move

Теперь это будет работать за O(1), если класс поддерживает move семантику.

```
template <typename T>
void swap(T& x, T& y) {
    T tmp = std::move(x);
    x = std::move(y);
    t = std::move(tmp);
}
```

## наивная реализация std::move

Это неправильная реализация, но она покрывает все случаи выше, а также большинство других, кроме чего-то хитрого.

```
template <typename T>
T&& move(T& x) {
    return static_cast<T&&>(x)
}
```

По сути *move* это просто rvalue каст.

Пример на понимание.
```
string s = "abs";
std::move(s);
``` 

Что произойдет? Ничего, потому что сама функция ничего не делает, она даже на стадие выполние это 0 действий, это только для этапа компиляцие, чиста ради то куда в какую версию оптравится.

А что если?
```
template <typename T>
T&& move(T x) {
    return static_cast<T&&>(x)
}
```

Тут бы создавался новый объект, а также возвращалась битая ссылка.

```
template <typename T>
T move(T& x) {
    return static_cast<T&&>(x)
}
```

Тут на стадие выхода из функции у нас из rvalue объекта создавался rvalue объект, т.е вызовался move конструктор. Но мув не должен вызывать конструктор.

## 10.3 Formal definitions of lvalue and rvalue.

**Lvalue and rvalue are categories of expressions, not types!!**

Формальное определение это таблица снизу (оператор запятая имеет такой же тип, который возвращает правый чел)

У тернарного оператора, если у него обе части lvalue, то он lvalue, а иначе rvalue.

|lvalue     |                   rvalue |
|:---------:|:------------------------:|
|x, "abc",  |3, 'a', 2.0f,             |
| =, +=, -= |true, +, -, %, &,         |
| *=, ..    |^, !, ~, &&, "||", ...    |
| ++expr    | expr++                   |
|*ptr, a[i] | &x                       |


*Замечание, это для фундаментальных типов*

*function called or cast-expresson : rvalue if return type is T&, otherwise if type is T or T&&*

## 10.4 rvalue ссылки их свойства

1) Выше про функцию.
2) То что ее можно проинцилизировать rvalue выражением.

```
int x = 5;
int&& y = x; // CE expr x is lvalue
int&& y = 6; // OK
y = 7; // OK
int&& z = y; // CE expr y is lvalue
int&& z = std::move(y); // OK
int&& t = static_cast<int&&>(x);
t = 1; // OK x = 1
```