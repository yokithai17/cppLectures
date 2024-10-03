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
  function(auto&& func)
      :


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