#include <iostream>
#include <vector>
#include <string>
#include <scoped_allocator>

// 9.6. Scooped allocators.
template <typename Alloc>
struct scooped_allocator_adaptor {
    Alloc alloc;

    template <typename T, typename... Args>
    void construct(T* ptr, const Args&...) {
        if constexpr (std::uses_allocator_v<T, Alloc>) {
            using InnerAlloc = typename T::allocator_type;
            alloc.construct(ptr, args..., InnerAlloc(alloc)); //???
        } else {
            alloc.construct(ptr, args...);
        }
    }
};


template <typename T>
using MyAlloc = std::allocator<T>;

int main() {
    using MyString = std::basic_string<char, std::char_traits<char>, MyAlloc<char>>;

    MyAlloc<MyString> alloc;
    std::vector<MyString, MyAlloc<MyString>> v(alloc);

    v.push_back("abs");
    v.push_back("cde");
}

