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

template <typename Alloc>
struct allocator_traits {
    template <typename U, typename... Args>
    static void construct(Alloc& alloc, U* ptr, const Args&... args) {
        if constexpr (/* Alloc has method construct*/) {

        } else {
            new (ptr) U(args...);
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