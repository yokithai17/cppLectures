#include <istream>

// XII. Smart Poiners

// 12.1 unique_ptr

template <typename T>
struct default_delete {
    void operator()(T* ptr) const {
        delete ptr;
    }
};

template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
    T* prt;
    [[ no_unique__address ]] Deleter del;

public:
    unique_ptr(T* ptr, Deleter del) 
        : ptr(ptr), del(del) {}

    unique_ptr(T* ptr) : ptr(ptr) {}

    ~unique_ptr() {
        del(ptr);
    }

    unique_ptr(const unique_ptr&) = delete;

    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& other)
        : ptr(other.ptr), del(std::move(other.del)) {
        other.ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) {
        if (this != &other) {
            del(ptr);
            ptr = other.ptr;
            other.ptr = nullptr;
            del = std::move(other.del);
        }

        return *this;
    }
     
    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }
};