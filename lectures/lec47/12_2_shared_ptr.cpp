#include <iostream>
#include <memory>

// 12.2. shared_ptr internals

template <typename T>
class shared_ptr {
    T* ptr;
    BaseControlBlock* cb;

    struct BaseControlBlock {
        size_t shared_count;
        size_t weak_count;
        virtual ~BaseControlBlock() = 0;
    };

    template <typename U, typename Deleter, typename Alloc>
    struct ControlBlockRegular: BaseControlBlock {
        Deleter del;
        Alloc alloc;
    };

    template <typename U, typename Alloc>
    struct ControlBlockMakeShared : BaseControlBlock {
        U value;
        Alloc alloc;
    };

    template <typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&...);

    shared_ptr(ControlBlock* cp); // TODO

    template <typename Deleter>
    shared_ptr( )

public:
    shared_ptr(T* ptr)
        : ptr(ptr), count(new size_t(1)) {
        if constexpr (std::is_base_of_v<enable_shared_from_this<T>, T>) {
            ptr->sptr =  *this;
        }
    }
    
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

template <typename T, typename Alloc, typename... Args>
shared_ptr<T> allocate_shared(const Alloc& alloc, Args&&... args) {
    using BlockAlloc = typename std::allocator_traits<Alloc>
            ::rebind_alloc<ControlBlockMakeShared<T, Alloc>>;
    BlockAlloc ba = alloc;
    auto* ptr = ba.allocate(1);
    //aloc traits
    ba.construct(ptr, 1, 0, std::forward<Args>(args..)..., alloc);
    return ...; // TODO private shared_ptr construct.
}

// 12.4 weak ptr

// 12.5 enable_shared_from_this
// CRIP

struct S : public std::enable_shared_from_this<S> {
    std::shared_ptr<S> getObject() {
        return shared_from_this();
    }
};

template <typename T>
struct enable_shared_from_this {
    shared_ptr<T> sptr;
    shared_ptr<T> shared_from_this() const;

    enable_shared_from_this() {}


    template <typename U>
    friend class shared_ptr;
};