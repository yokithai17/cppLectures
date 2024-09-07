#include <iostream>
#include <memory>
#include <array>

// IX. Allocators and memory managment.

// 9.1 Idea and Basic usage of alllocators.

/*
*
* Container -> Allocator -> operator new -> malloc -> OS
*
*/

// 9.2. StateFul allocators and allocators_traits.

// PoolAllocatrs
// StackAllocator
// FreeListAllocator

// 10.6 std::forward implementations

template <typename T>
struct is_lvalue_reference: std::false_type {};

template <typename T>
struct is_lvalue_reference<T&>: std::true_type {};

template <typename T>
T&& forward(std::remove_reference_t<T>& value) {
    return static_cast<T&&>(value);
}

template <typename T>
T&& forward(std::remove_reference_t<T>&& value) {
    static_assert(!std::is_lvalue_reference_v<T>);
    return static_cast<T&&>(value);
}

template <typename T>
std::remove_reference_t<T>&& move(T&& value) {
    return static_cast<std::remove_reference_t<T>&&>(value);
}

template <typename Alloc>
struct allocator_traits {
    template <typename U, typename... Args>
    static void construct(Alloc& alloc, U* ptr, Args&&... args) {
        if constexpr (/* Alloc has method construct*/) {

        } else {
            new (ptr) U(std::forward<Args>(args)...);
        }
    }
};

template <typename T>
struct allocator {
    T* allocate(size_t count) {
        return reinterpret_cast<T*>(new char[count * sizeof(T)]);
    }

    void deallocate(T* ptr, size_T) {
        delete[] reinterpret_cast<char*>(ptr);
    }

    template <typename U>
    allocator(allocator<U>) {}

    template <typename U, typename... Args>
    void construct(U* ptr, const Args&... args) {
        new (ptr) U(args...);
    }
};


template <typename T, typename Alloc = std::allocator<T>>
class list {
    struct BaseNode {
        BaseNode* prev;
        BaseNode* next;
    };

    struct Node : BaseNode {
        T value;
    };

    BaseNode fakeNode;
    size_t count;
    typename Alloc::rebind<Node>::other alloc;

    list(const Alloc& alloc)
        : fakeNode(), count(), alloc(alloc) {}
};


int main() {
    std::array<char, 1000000> arr;
    //StackAllocator<int>(arr);
    //std::list<int, StackAllocator<int>> lst;
}