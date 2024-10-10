#include <cstddef>
#include <iostream>
#include <functional>
#include <type_traits>

// 14.5. std::function usage and naive implementation

int square(int x) {
  return x * x;
}

struct AddOne {
  int operator()(int x) {
    return x + 1;
  }
};

struct Starnge {
  int c = 7;
  int moduloSeven(int n) {
    return n % c;
  }
};

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
  ~function() {
    destroy_ptr(fptr);
  }

  Ret operator()(Args... args) const {
    return invoke_ptr(fptr, std::forward<Args>(args)...);
  }

private:
  static const size_t BUFFER_SIZE = 16;
  Base* fptr;
  alignas(std::max_align_t) char buffer[BUFFER_SIZE];

  using invoke_ptr_t = Ret(*)(void*, Args...);
  using destroy_ptr = void(*)(void*);

  destroy_ptr_t destroy_ptr;
  invoke_ptr_t invoke_ptr;

  template <typename F>
  static void destroyer(F* fptr) {
      if constexpr (sizeof(F) > BUFFER_SIZE) {
          delete fptr;
      } else {
          fptr->~F();
      }
  }

public:
  template <typename F>
  static Ret invoke(F* fptr, Args...) {
    return (*fptr)(std::forward<Args>(args)...);
  }

  template <typename F>
  function(const F& func)
    : invoke ptr(reinterpret_cast<intvoke_ptr_t>(
        &invoker<F>))
      , destroy_ptr(reinterpret_cast<destroy_ptr_t>(&destroyer<F>))
  {
    if constexpr (sizeof(F) > BUFFER_SIZE) {
      fptr = new F(func);
    } else {
      new (buffer) F(func);
      fptr = buffer;
    }
  }

};

struct NoCopyble {
  std::unique_ptr<int> p;

  int operator()(int x) {
    return x + *p;
  }
};

int main() {
  /*
  std::function<int(int)> f = square;

  f = AddOne();

  int c = 10;
  f = [&c](int x) { return x * 10; };

  std::function<int(Starnge&, int)> ff = &::Starnge::moduloSeven;

  Starnge s{5};
  ff(s, 1);

  auto f2 = f;
  */
}