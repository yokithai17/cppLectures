#include <iostream>
#include <vector>
#include <iterator>
#include <type_traits>

// 8.4 Iterators implementation const and reverse-iterators

template <typename T>
class vector {
    T* arr;
    size_t sz;
    size_t cap;
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
};