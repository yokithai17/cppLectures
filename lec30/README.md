# Реализация итераторов. Адаптеры над итераторами

Для указателя нам нужно реализовать отдельный iterator_traits

## std::distance and std::advance

std::distance нам скажет за сколько шагов мы дойдем от 1 до другого итератора. Если RandomAccess O(1), иначе за O(n)

```
template <typename Iterator>
typename std::iterator_traits<Iterator>::difference_type
distance(Iterator first, Iterator) {
    if constexpr (std::is_base_of_v<
        typename std::iterator_traits<Iterator>::iterator_category,
        std::random_access_iterator_tag
        >) {
        return last - first;
    }

    typename std::iterator_traits<Iterator>::difference_type i = 0;
    for (; first != last; ++i, ++first);
    return i;
}
```

## std::prev and std::next

Очевидно, что они делают

## iterators implementations on vector

Вообще итераторы это класс внутри контейнера.

```
private:
    template <bool IsConst>
    class base_iterator {
    public:
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>
        using value_type = T;

    private:
        pointer ptr;
        base_iterator(T* ptr): ptr(ptr) {}

    public:
        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;

        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }

        base_iterator& operator++() {
            ++ptr;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator copy = *this;
            ++ptr;
            return copy;
        }
    };
public:
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
```

Соответсвенно для begin and end мы должны определить по разному.

## reverse_iterator

Это отдельный класс в стандартной библиотеке, в отличие от итератора. Можно сказать, что это адаптор над итератором.

Его можно создать от итератора. Соответственно 

``` 
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>
```

