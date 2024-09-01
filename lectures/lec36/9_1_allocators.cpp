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

// 9.3 Allocator-aware containers.

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
        //return reinterpret_cast<T*>(new char[count * sizeof(T)]);
        return operator new(count * sizeof(T));
    }

    void deallocate(T* ptr, size_T) {
        //delete[] reinterpret_cast<char*>(ptr);
        operator delete(ptr);
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

/*
    vector& operator=(const vector& other) {
        if constexpr (std::alocator_traits<Alloc>::propagate_on_container_copy_assinment::value) {
        
        }

        Alloc newAlloc = std::alocator_traits<Alloc>::propagate_on_container_copy_assinment::value
            ? other.alloc_ : alloc_;

        T* newarr = std::alocator_traits<Alloc>::allocate(newAlloc, other.cap_);
        size_t  i = 0;
        try {
            for (; i < other.sz_; ++i) {
                std::allocator_traits<Alloc>::construct(newAlloc, newarr + i, other[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                std::allocator_traits<Alloc>::destroy(newAlloc, newarr + j);
            }
            std::allocator_trairs<Alloc>::deallocate(newAlloc, newarr, other.cap_);
            throw;
        }

        for (size_t i = 0; i < sz_; ++i) {
            std::allocator_traits<Alloc>::destroy(alloc_, arr + i);
        }
        std::allocator_traits<Alloc>::deallocate(alloc_, arr_, cap_);
        alloc_ = newAlloc; // nothrow
        arr_ = newarr;
        sz_ = other.sz_;
        cap_ = other.cap_;
    }
*/


int main() {
    std::array<char, 1000000> arr;
    //StackAllocator<int>(arr);
    //std::list<int, StackAllocator<int>> lst;
}