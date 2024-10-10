# Лекция 56. Проблемы std::function. std::bind, std::reference_wrapper

Вспомним, что мы хотели сделать: маленькие оьбхекты хранить на стекке, а также чтобы когда ьыл кол обхекта мы сразу делали а не лазилии в разные виртнуальные таблицы

Вообще илюха нас обманул

```c++
  template <typename F>
  static Ret invoke(F* fptr, Args...) {
    return (*fptr)(std::forward<Args>(args)...);
  }
```

```c++
template <typename F>
function(const F& func)
    : invoke ptr(reinterpret_cast<intvoke_ptr_t>(
        &invoker<decltype(func)>))
```

Если у нас маленький object so we put pointer to fptr contain BUFFER

```c++
void* fptr;

  template <typename F>
  function(const F& func)
    : invoke ptr(reinterpret_cast<intvoke_ptr_t>(
        &invoker<F>))
        {
          if constexpr (sizeof(F) > BUFFER_SIZE) {
            fptr = new F(func);
          } else {
            new (buffer) F(func);
            fptr = buffer;
          }
        }
```

Только теперь у нас проблема в том, что делаем делет у void*. Давайте заведем статическую функцию удаления.

```c++
using destroy_ptr = void(*)(void*);

destroy_ptr_t destroy_ptr;

template <typename F>
static void destroyer(F* fptr) {
    if constexpr (sizeof(F) > BUFFER_SIZE) {
        delete fptr;
    } else {
        fptr->~F();
    }
}

~function() {
    destroy_ptr(fptr);
}
```

Теперь destroy_ptr мы должны. Добавим в конструктор

```c++
, destroy_ptr(reinterpret_cast<destroy_ptr_t>(&destroyer<F>))
```

```c++
  Ret operator()(Args... args) const {
    return invoke_ptr(fptr, std::forward<Args>(args)...);
  }
```

Это работает (как минимум над функцие тривиальной), заметим чтобы реализовать копированиу мувы и тд нам нужно создавать статические функцие, потому что по факту все что мы делаем это просто создаем ви таблицу руками.

Проблема состоит в том, что когда накидаем большое количество таких птров у нас объект станет большим, но мы хоимт, чтобы только invoke работал быстро.

Давайте с вами сделаем, что типа control block with all pointer init except inoker_ptr. И будем хранить птр на этот контрол блок.

Хотим ли мы потдержать кастомный аллокатор? Нет начиная с c++17. Так как это гроб.

## No copy object and std::function

```c++
struct NoCopyble {
  std::unique_ptr<int> p;

  int operator()(int x) {
    return x + *p;
  }
};

std::function<int(int)> = NoCopyble();
```

И вот с таким страктором мы хотим создать фанкшион. У нас будет CE так как у нас нет Copy construct причем самое интересно, что мы даже создать не можем такой объект

Потому, что у нас инстацируется все статические методы и пойнтеы на них (вспоминаем реализацию).

ПОЛУЧАЕТСЯ фанкшион не поддерживает move only functor.

НО в комитете не дураки, поэтому там добавлен std::move_only_function класс (since c++23)

## std::bind and bind-object

Вот мы же хотим сделать функцию, в которой зафиксирован одна из перемееных. МЫ уже ревлизовали на шаблонах. но есть более элегандный способ.

```c++
int sum(int x, int y) {
    return x + y;
}

int diff(int x, int y) {
    return x - y;
}

int main() {
    auto f = std::bind(sum, 5, std::placeholders::_1);

    auto g = std::bind(diff, std::placeholders::_1, 5);
    std::cout << f(3) << ' ' << g(3);
}
```

Во что же превратился auto? К какомо то std::bind, похожую на std::function. std::bind<int (*(place_holder, int))(int, int)>

самое что интересное мы можем передать эту вещь в std::function

```c++
std::function<int(int)> = std::bind(sum, 5, std::placeholders::_1);
```

## reference_wrapper

Мы бы хотели передовать в std::bind что нибудь по указателю
к  примеру

```c++
int x = 5;
std::funciton<int(int)> g = std::bind(diff, std::placeholders:::_1, std::ref(x));

std::cout << g(3); // -2
int x = 1;
std::cout << g(3); // 2;
```

Фокус. Как мы хотим передать ссылку, а не копию по общим правилам, когда мы передаем там делается remofe reference

Вот и есть такой класс reference_wrapper, который является копируемый ссылке. ВОобщем он хранит в себе пойнтер, он неявно преобразовывыет в ссылку.

А что такое std::ref, прсото функция, Которая возвращает reference wrapper на этот тип.

## string_views, views

Что такое string view, это типа такая штука, которая смотрит на строку но не владеет.

Короче там типо 2 указателей

Вот а в с++20 появилась std::span, которая позволяет наблюдать над каким-то контейнером.

И вообще когда мы работаем над диапозоном можем над таким работоать.

КОРОЧЕ вот мы приходим к некотрому концепту range и вообще новое namespace range

Типо у нас есть некотрый ренйд и мы хотим смотерть на него имзпользуя некоторые фильтры.

И вот к над ним опредедл Pipe (конвеер как bash)

```c++
auto even = [](int x) { return 0 == i % 2; };
auto square = [](int x) { return i * i; };

for (int i : std::views::iota(0, 6)
            | std::views::filter(even)
            | std::views::transform(square)) {
    std::cout << i; << ' ';
}
```

Есть еще sorted view, zip view и другие приколы.
