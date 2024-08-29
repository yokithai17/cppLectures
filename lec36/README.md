## Allocator-aware контейнеры. Перегрузка new и delete

Что должен делать контейнер когда он копируется?
- Он должен копировать аллокатор или создавать новый по умолчанию

Иногда нам хотелось одно, иногда другое.

И для этого ему должен сказать сам аллокатор

``` select_on_container_copy_constructor ```

Вот это ему должен сказать аллокатор. Он возвращает аллокатор
по значение, и он вызывается контейнером, когда копируется

Если не определить в своем аллокаторе этот метод, то тогда
***allocator_trairs*** сам определит этот метод и будет
возвращать ему исходный аллокатор.

``` propogate_on_container_copy_assigment ```

Это мета функция, в которой лежит ***bool*** и если там
лежит ***True*** то присвоить аллокатор, а иначе
не трогать аллокатор

Мы должны старые объекты удалить своим аллокатором, а новые 
объекты создать новым аллокатором.

Все стандартыне аллокаторы равны между собой

есть темкак is_always_equal

## Operators new and delete overloading

Когда вы вызываете оператор ***new***, то происходит 2 вещи
1) Выделение памяти
2) И вызов конструктора

И переопределить можно только первую часть, то есть то
кто возвращает ***pointer***

Оператор new[] по умолчанию вызывает new

Почему в векторе вызывется new, а new[]?

```
T* allocate(size_t count) {
    // cringe 
    //return reinterpret_cast<T*>(new T[sizeof(T) * count]);

    return operator new(count * sizeof(T));
}
```

operator new(size_t) как раз первая часть от new

Как оператор delete[], знает сколько деструкторов ему кинуть
Он дополнительно хранит (это требует 8 byte), где-то перед массивом


```
void deallocate(T* ptr, size_T) {
    //delete[] reinterpret_cast<char*>(ptr);
    operator delete(ptr);
}
```

## Example 

```
#include <new>

struct Example {
    int* x;
    Example(): x(new int) {}
    ~Example() { delete x; }
};

int main() {
    // OK
    Example* example = reinterpret_cast<Example*>(new char[sizeof(Example)]);
    new (example) Example();

    //Example* example = new Example[1]; // Not OK

    example[0].~Example();
    operator delete[](example);
}
```

Когда мы создавали example, как массив чаров, он был не сдвинут
и соотвественно в оператор делит квадратные скобки попадал
тот адресс, который нужен

А когда мы создавали через new[], то адресс был сдвинут на
т.е он неявно всегда свдигает 
