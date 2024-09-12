#  Внутреннее устройство shared_ptr и weak_ptr

## unique_ptr implements

```c++
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
        : ptr(other.ptr) {
            other.ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
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
```

## shared_ptr internals

Вот что у нас должен хранить шеред птр? Давайте поймем как он работает.
Вот как бы юз кейс, причем они все укзывают на 5, потому п2 указывает на8
Ну вообщем понятно что происходит

```c++
auto p = std::shared_ptr<int>(new int(5));

auto p2 = p;

auto p3 = p;

p2 = new int(8);
```

Ясно, что если сделать ```int count``` то это бред и так
не сработает)))

Также если сделать с ```static int count``` потому что иначе
он будет на каждый экземпляр

ну ясно теперь что нужно сделать?

Вот реализация на 3. Шеред птр очень бедного человека

```c++
template <typename T>
class shared_ptr {
    T* ptr;
    size_t* count;

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
```


## std::make_shared 

make_SOMETING позволяет создаться от элемента сама выделев память
и вообще обычно она и изпользуется а не сишный пойнтер конструктор

Вообще мейк темка придумана для того чтобы (1 мотивация) вызов **new**
делается всего 1 раз в отличие от констркуторов, где 2 раза
вызывается **new**.

```c++
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    ...
}
```

И вот на самом деле мы заведем некоторую конструкцию **Control_block**
который будет как раз хранить ребят.

```c++
struct ControlBlock {
    T value;
    size_t count;
};
```

Теперь конечно не понятно как хранить поля.... Т.е если у нас
count == nullptr то мы знаем оффсет где лежит control block 
но это тоже немного проблема

2 мотивация для создание make_unique... Рассмотрим такую темку

until c++17 компилятор мог не последовательно делать... К примеру
1 new 2 g g вызвало исключение, для этого и было make_unique (c++14)

Но начиная c++17 сказали что нельзя не последовательно

```c++
f(unique_ptr<int>(new int(5)), g());
```


## std::weak_ptr

Вот давайте мы теперь будем делать красно черное дерево на
умных указателях.

У нас циклические зависимости и все это на shared_ptr ничего
не удаляется

Поэтому мы хотим еще какие то пойнтере Для ершение этой 
проблемы

weak_ptr это такой pointer, который не учитывается при
подсчете количество pointer ов а смотеррть что под ним
можно

И вот теперь если сделать указатели на родителей weak_ptr
то все схлопнется когда корень умрет

Какой функционал мы хотим от weak_ptr
1) Создаваться от shared_ptr
2) Забавно, но его нельзя разыименовать
3) у weak_ptr можно попросить shared_ptr функция *lock*

Тут weak ptr поймет, что у нас удалился челик

И как оказывается нам нужно хранить shared_count, а также
weak_count

НА САМОМ деле у нас будет разновидность структуры
control block

```c++
struct ControlBlock {
    size_t shared_count;
    size_t weak_count;
};

struct ControlBlockWithObject : ControlBlock {
    T value;
};
```

Давайте теперь поймем как менять нам деструктор

Правильно ли что каждый каунт = 0

В shared_ptr если стал shared_count = 0 то удалить объект. А если
weak_count = 0, то удалить control block.

Проблема тут состоит в том, что хоть и деструктор на T 
мы отправили, но память не убирается


