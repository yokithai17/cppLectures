# Внутреннее устройство deque. Итераторы. Виды итераторов

## poiner and reference invalidation

```
vector<int> v(10);
int* p = &v[5];
v.push_back(1); // pointer invalidation
cout << *p;
```

Что будет? UB, так как у нас при пуш беке может быть реалокация вектора. Это явление называется инвалидация указателя. Здесь почти наверное так и будет.

А что будет в этом случае

```
vector<int> v(10);
int& x = v[5];
v.push_back(1); // reference invalidation
cout << x;
```

## 8.2 std::deque internals

у нас есть массив указателей, который ссылаются на бакеты.
Т.е мы храним ```T** arr;```, причем здесь у нас сырая память.

Когда мы создали дек, у нас не должна аллоцироваться память, при добавление 1 элемента мы создаем бакет, в который кладем элемент, причем все остальные это просто сырая память.

Если делаем ```push_front```, мы кладем перед началом. Соответсвенно нам нужно еще хранить пару указателей.
```(i, j)``` для начало дека, а также ```(i, j)``` для конца.

Поймем, что мы добились того, чего хотели. Теперь у нас указатели не инавлидируются.

Также за O(1) мы очевидно поймем, что за элемент, так как у нас бакет размера заранее известный.

Теперь поймем, что ```std::stack, std::deque, std::priorit_queue``` это просто адаптеры над деком.

Что такое ```std::stack```?

```
template <typename T, typename cont = std::deque<T>>
class stack;
```

Почему у нас медот ``` void pop() ``` *void*, а не T?

1) Каждый раз, когда мы ходим попать, у нас копировался объект.

## Iterators and their categories

Иттератор это такой тип, который позволяет делать обход последовательности. Некое обобщение указателей.

На cpp-reference есть формальное определение.

Поймем какие есть категорие.

```
graph TD;
    InputIterator-->| forward_list u_set u_map|ForwardIterator;
    ForwardIterator-->|-- set map list|BidirectionalIterator;
    BidirectionalIterator-->|+= vec <, > deque|RandomAccessIterator;
    RandomAccessIterator-->|since c++17 vec, arr|CantigiusIterator;
```

## range based for

```
set<int> s;
for (int x : s) {
    ...
}
```

Это синтаксический сахар since c++17;

```
for (std::set<int>::iterator it = s.begin(); it != s.end(); ++it) {
    ...
}
```

