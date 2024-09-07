#include <iostream>
#include <vector>
#include <iterator>
#include <type_traits>

// 8.4 Iterators implementation const and reverse-iterators

// 10.7 push_back implemnetatiom with move semantics support.


template <typename T, typename allocator>
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
    void reserve(size_t newcap) {
        if (newcap <= cap) {
            return;
        }

        T* newarr = std::allocator_traits<allocator>::allocate(alloc,, newcap);
        size_t index = 0;
        try {
            for (; index < sz; ++index) {
                std::allocator_traits<allocator>::construct(alloc,
                 newarr + index, std::move_if_noexcept(arr[index]));

            } catch (...) {
                for (size_t oldIndex = 0; oldIndex < index; ++oldIndex) {
                    std::allocator_traits<allocator>::destroy(alloc, newarr + oldIndex);
                }
                std::allocator_traits<allocator>::deallocate(alloc, newarr, newcap);
            }
        }
    }

    void push_back(const T& value) {
        emplace_back(value)
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (sz == cap) {
            reserve(cap > 0 ? cap * 2 : 1);
        }

        std::allocator_traits<allocator>::construct(alloc, arr + sz,
                                            std::forward<Args>(args)...);
        ++sz;
    }

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
};