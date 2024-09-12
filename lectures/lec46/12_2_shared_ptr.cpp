#include <iostream>
#include <memory>

// 12.2. shared_ptr internals

template <typename T>
class shared_ptr {
    T* ptr;
    СontrolBlock* count;

    struct ControlBlock {
    size_t shared_count;
    size_t weak_count;
    };

    struct ControlBlockWithObject : ControlBlock {
        T value;
    };

    template <typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&...);

    shared_ptr(ControlBlock* cp); // TODO

public:
    shared_ptr(T* ptr)
        : ptr(ptr), count(new size_t(1)) {}
    
    shared_ptr(const shared_ptr& other)
        : ptr(other.ptr) , count(other.count) {
            ++count;
    }

    ~shared_ptr() {
        if (!count) {
            return;
        }

        --*count;
        if (!*count) {
            delete count;
            delete ptr;
        }
    }
};

// 12.3 make_shared, make_unique

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    auto* p = new typename shared_ptr<T>::ControlBlock{T(std::forward<Args>(args)...), 1};
    return shared_ptr<T>(p);
}