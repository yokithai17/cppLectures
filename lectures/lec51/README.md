#  Requires-выражения. Концепты

## Есть еще оператор requires

мы бы хотели нормальным языком описать требование.

```c++
template <typename T>
void test() {
  std::cout << requires(T a, T b) { a + b; };
}
```

Вот мы написали requires expression, и то что написано внутри проверяется на компилируемость

```c++
template <typename T>
requires requires(T a, T b) { a + b; }
void test(const T& a, const T& b) {
  return a + b;
}
```

давайте еще примеров рассмотрим

```c++
template <typename T>
requires requires(T a, T b) { std::sort(a, b); }
void test(const T& a, const T& b) {}
```

И запустм над vector, то будет ок, а если над list, тоже окей, но почему? Потому, что он проверяет, что можно вызвать его над типами, а проверка уже внутри функции над тэгами итераторами.

**То что написано под requiers не проверяется на истинность, а только на компилируемость**

**Но внутри requires можно писать requires**

У нас вообще есть 4 типа вложенности requires
1) Simple req
2) Nested req
3) Type req

```c++
template <typename T, size_t N>
requires requires(T a) {
  requires sizeof(T) == a;
}
void test(const T& a) {}
```

```c++
template <typename T, size_t N>
requires requires(T a) {
  requires sizeof(T) == N;
  typename T::value_type;
}
void test() {}
```
Вот допустим мы хотим сделать хэш таблицу и првоерить, что есть хэш над T

```c++
template <typename T>
requires requires(T a) {
  typename std::hash<T>; // ВСЕГДА БУДЕТ
  std::hash<T>()(a);     // А вот так уже нет
}
```

Некоторые requires проверяются за нас


## conspets

Вот как будто когда мы передаем шаблонные параметры с названием здесь, мы хотим некий интерфейс

```c++
template <typename InputIterator, typename Predicate>
requires requires (InputIterator it, Predicate p) {
  ++it;
  *it;
  p(*it);
}
InputIterator find_if(InputIterator beg, InputIterator end, Predicate p) {
  for (auto it = beg; it != end; ++it) {
    if (p(*it)) {
      return it;
    }
  }
  return end;
}
```

И вот для этого можно сделать концепты

```c++
template<typename T>
concept InputIterator = requires(T a) {
    ++a;
    *a;
};


template<typename It, typename Predicate>
requires InputIterator<It>
&& requires(Predicate p, It it) { p(*it); }
It find_if(It beg, It end, Predicate p) {
  for (auto it = beg; it != end; ++it) {
    if (p(*it)) {
      return it;
    }
  }
  return end;
}
```

Но мы же еще хоте ли бы, чтобы ++a что то возврощало (сразу хочу сказать, что same_as неправильно реализован)

```C++
template<typename T, typename U>
concept same_as = std::is_same_v<T,U>;

template<typename T>
concept InputIterator = requires(T a) {
    ++a -> same_as<T&>;
    *a;
};
```

И вообще мы можем концепты подставить

```c++
template<typename T>
concept InputIterator = requires(T a) {
    {++a} -> std::same_as<T&>;
    typename std::iterator_traits<T>::value_type;
    {*a} -> std::convertible_to<typename std::iterator_traits<T>::value_type>;
};


template<InputIterator It, std::predicate<decltype(*std::declval<It>())> Pred>
It my_find_if(It beg, It end, Pred p) {
  for (auto it = beg; it != end; ++it) {
    if (p(*it)) {
      return it;
    }
  }
  return end;
}
```

Но помните, что можно писать **auto** в принимаемом типе

```c++
auto my_find_if(InputIterator auto beg, decltype(beg) end
               , std::predicate<decltype(*std::declval<decltype(*beg)>())>  auto p) {
  for (auto it = beg; it != end; ++it) {
    if (p(*it)) {
      return it;
    }
  }
  return end;
}
```
