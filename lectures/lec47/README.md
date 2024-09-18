# Type erasure, std::any. Окончание реализации shared_ptr

## enable_shared_from_this

Предстватьте у нас есть такая ситуацияция, который может возвращаеть укзатель этой структуры, и представте нам нужно вернуть указатель на себя. Вот вы знаете структурой S, что вы были созданы структурой make_shared, и вот как мне достать shared_ptr, и мы не можем
сделать **std::shared_ptr(this)**, потому что мы создадим еще 1 семейство пойнтеров.

И вот для этого нам нужно унаследоваться от std::

```c++
struct S : private std::enable_shared_from_this<S> {
    std::shared_ptr<S> getObject() {
        return shared_from_this();
    }
};
```

Давайте поймем как реализовать теперь эту структуру.

```c++
template <typename T>
struct enable_shared_from_this {
    shared_ptr<T> sptr; // weak_ptr<T> sptr; OK
    shared_ptr<T> shared_from_this() const;

    enable_shared_from_this() {}


    template <typename U>
    friend class shared_ptr;
};
```

И теперь мы сделаем так... Изменим конструктор shared_ptr добавив несколько строчек

```c++
if constexpr (std::is_base_of_v<enable_shared_from_this<T>, T>) {
    ptr->sptr =  *this;
}
```

Однако, что если мы enable shared from this, но у нас кто то создал просто объект, не через
make_shared то как бы да 

В какой момент структура S уничтожиться, ведь у нас поле shared_ptr на shared_ptr и мы закольцивались => мы должны хранить weak_ptr

## Deleter для shared_ptr

Вот у нас shared ptr, у которого всего 1 шаблонный тип. Как нам бороться теперь с кастомным делитером и вот теперь у нас НОВАЯ ИДЕЯ type erasure

## std::any 

Есть такая штука как std::any, которая может присваивать любой тип, однако чтобы достать нужно будет кастовать

```c++
int main() {

    std::any a = 5;
    std::cout << std::any_cast<int>(a) << '\n';

    a = "abs";
    std::cout << std::any_cast<const char*>(a) << '\n';

    std::vector<int> v{1, 2, 3};

    a = v;
    std::cout << std::any_cast<std::vector<int>&>(a)[0] << '\n';

    // std::any_cast<int>(a) std::bad_any_cast
}
```

## type erasure

Идеома **type erasure** заключается в этом  у нас есть Base and Derived

```c++
    struct Base {
        virtual ~Base() = 0;
    };
    
    template <typename T>
    struct Derived: public Base {
        T value;
        Derived(const T& value): value(value) {}
        Derived(T&& value): value(std::move(value)) {}
    };

    Base* ptr;
```

И вот теперь, когда мы удалим ptr, будет вызываться правильная темка.

И вот теперь на основе наших знаний 

```c++
class any {
private:
    struct Base {
        virtual Base* getCopy() const = 0;
        virtual ~Base() = default;
    };
    
    template <typename T>
    struct Derived: public Base {
        T value;
        Derived(const T& value): value(value) {}
        Derived(T&& value): value(std::move(value)) {}
    
        Base* getCopy() const override {
            return new Derived(value);
        }
    };

    Base* ptr;

public:
    template <typename T>
    any(const T& value): ptr(new T(value)) {}

    any(const any& other): ptr(other.ptr->getCopy()) {}

    ~any() {
        delete ptr;
    }
};
```

## std::any_cast

```c++
template <typename T>
T any_cast(any& a) {
    auto* p = dynamic_cast<any::Derived<std::remove_reference_t<T>>*>(a.ptr);
    if (!p) {
        throw std::bad_any_cast;
    }

    return p->value;
}
```

## shared_ptr Deleter

А как же теперь с этим знанием реализовать подмену делитора

Заметим, что если мы создаеся через make shared, то тогда у нас дефолтный делитор, но может быть кастомный аллократор.

```c++
    struct BaseControlBlock {
        size_t shared_count;
        size_t weak_count;
        virtual ~BaseControlBlock
    };

    template <typename U, typename Deleter, typename Alloc>
    struct ControlBlockRegular: BaseControlBlock {
        Deleter del;
        Alloc alloc;
    };

    template <typename U, typename Alloc>
    struct ControlBlockMakeShared : BaseControlBlock {
        U value;
        Alloc del;
    };
```

Теперь в ран тайме мы понимаем, что у нас лежит под control block, мы также должны поддержать get-copy and move semantic for control block.

## allocate shared

```c++
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
```

## deallocate

Поймем теперь как нам уничтожаться, когда мы в деструкторе контрол блока находися, и у нас 2 операции
1) когда умирает shared_ptr, но еще живы weak_ptr, нужно на аллокаторе позвать дестрой на T
2) Когда умиарет последний shared_ptr, нужно вызвать, чтобы умер контрол блок, достать из контро блока аллокатор, который удилит сам контрол блок, после чего все захочется

Возможно мы захотим сделать **delete this**, и это можно, когда у нас последнее действие 
