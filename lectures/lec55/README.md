# std::function и ее реализация. std::invoke

## std::function

```c++
int square(int x) {
  return x * x;
}

struct AddOne {
  int operator()(int x) {
    return x + 1;
  }
};

int main() {
  std::function<int(int)> f = square;

  f = AddOne();

  int c = 10;
  f = [&c](int x) { return x * 10; };

  auto f2 = f;
}
```

Причем все хорошо, насчет удаление полей при вызове деструкторе...

А все ли мы рассмотрели с коллабле? Нет, мы не рассмотрели передачу метода класса. Вот будет ли рабоать следующий код? Нет потому, что там еще неявно передается this, указаетель на объект S

```c++
struct Starnge {
  int moduloSeven(int n) {
    return n  % 7;
  }
};

std::functionla<int(int)> f = &Strange::moduloSeven;
```

Для этого нужен другой **std::functional**, в который будет передаваться уже ссылка на объект класса.

```c++
std::functional<int(Strange&, int)> ff = &Strange::moduleSeven;
```

Еще коллабле считается полем. Ну вообще это полезтно, можно рассматриваит эта как проектор.

```c++
struct S {
  int x;
};

std::functional<int(S&)> f = &S::x;
```

Будет ли работаь? Правда ли, что рабоатет, если мы хотим передовать rvalue, а работаетм с Lvalue. Нет конечно.
Если const навесить, тоже не работоет, ведь мы собираемся передать lvalue да еще и константные, а передаем rvalue.

```c++
struct Strange {
  int c;
  int modulo(int n) && {
    return n % c;
  }
};

std::function<int(Strange&, int)> ff = &Strange::modulo;
```

К слову в template int(int) это валидный тип, это просто функция, однако мы не можем объявлять такие переменные.

## Первое приближение std::function

Как же нам только функцие принимать?

```c++
template <typename T>
class function;

template <typename Ret, typename... Args>
class function<Ret(Args...)> {
  ...
};
```


```c++
template <typename Ret, typename... Args>
class function<Ret(Args...)> {
private:
  struct Base {
    virtual ~Base() = default;
  };

  template <typename F>
  struct Derived: Base {
    F f;

    ~Derived() override = default;

    Derived(const F&): f(f) {}

    Derived(F&&): f(std::move(f)) {}
  };

public:
  function(auto&& func)
      : fptr(new Derived<
             std::remove_reference_t<decltype(func)> // TODO what if delctype(func) == const T&
             >(func)) {}

  ~function() {
    delete fptr;
  }

private:
  Base* fptr;
};
```

Почему оператор круглые скобки принимает по значению, потому что в них уже зашиты амперсанды.

```c++
Ret operator()(Args... args) const {
  ...
}
```

А что же мы будем делать? Давайте добавим теперь в бейзе метод, чтобы работал type erasuer

```c++
  struct Base {
    virtual Ret call(Args...) = 0;
    virtual ~Base() = default;
  };

  template <typename F>
  struct Derived: Base {
    Ret call(Args...args) override {
      return f(std::forward<Args>(args)...);
    }
  };

  Ret operator()(Args... args) const {
    return fptr->call(std::forward<Args>(args)...);
  }
```

Волнует ли нас теперь const наличчие или нет? Может ли call менять свои поля? Вот у нас то оператор то const, но он хранит пойнтер, т.е у нас стрелка будет константа, а не объект по ним.

А в причина тряски? Это небудет рабоать poiner to member-class

```c++
    Ret call(Args...args) override {
      if constexpr (std::is_member_function_pointer_v<F>) {
          // TODO call wh
        } else if constexpr (std::is_member_object_pointer_v<F>){
          // TODO
      } else {
        return f(std::forward<Args>(args)...);
      }
    }
```

На самом деле есть способ красивей, чем это штука.

## std::invoke

Это штука, которая вызывает callable объект правильно в зависимости от чего мы вызвались. Короче не нужно переибрать. Только нужно правильно передать ему параметры.

НО есть еще 2 большие проблемы и обе эти проблемы это эффективнсотьт
1) Каждое создание std::function это вызов оператора new.

## Small object optimization

Когда у вас маленькие объекты, то нам хочется, чтобы они лежали на стеке.

Мы хотим сделать, что то типа, если у нас меньше sizeof чего-то, то положить на буффер. Давайте, добавим что-то такое, но как мы хотим вызывать оператор круглые скобочки?

```c++
  static const size_t BUFFER_SIZE = 16;
  alignas(std::max_align_t) char buffer[BUFFER_SIZE];
```

мы хотим избавиться от virtul method, соответсвенно мы хотим решить проблему type erasure по-другому.

Предлагается запомнить пойнер вручную. Грубо говоря мы хотим запомнить v_table вручную.

```c++
template <typename T>
class function;

template <typename Ret, typename... Args>
class function<Ret(Args...)> {
private:
  struct Base {
    virtual Ret call(Args...) = 0;
    virtual ~Base() = default;
  };

  template <typename F>
  struct Derived: Base {
    F f;

    ~Derived() override = default;

    Derived(const F&): f(f) {}

    Derived(F&&): f(std::move(f)) {}

    Ret call(Args...args) override {
      if constexpr (std::is_member_function_pointer_v<F>) {
          // TODO call wh
        } else if constexpr (std::is_member_object_pointer_v<F>){
          // TODO
      } else {
        return f(std::forward<Args>(args)...);
      }
    }
  };

public:
  function(auto&& func)
      : // TO DO


  /*
      : fptr(new Derived<
             std::remove_reference_t<decltype(func)>
             >(func)) {}
  */
  ~function() {
    delete fptr;
  }

  Ret operator()(Args... args) const {
    return fptr->call(std::forward<Args>(args)...);
  }

private:
  static const size_t BUFFER_SIZE = 16;
  Base* fptr;
  alignas(std::max_align_t) char buffer[BUFFER_SIZE];

  using invoke_ptr_t = Ret(*)(Args...);
  invoke_ptr_t invoke_ptr;

public:
  static Ret invoke(Args...) {
    // TO DO
  }
};
```