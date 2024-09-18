#include <iostream>
#include <any>
#include <vector>

// 12.6 type erasure.

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
    
    template <typename T>
    friend T any_cast(any&);

public:
    template <typename T>
    any(const T& value): ptr(new T(value)) {}

    any(const any& other): ptr(other.ptr->getCopy()) {}

    ~any() {
        delete ptr;
    }
};

template <typename T>
T any_cast(any& a) {
    auto* p = dynamic_cast<any::Derived<std::remove_reference_t<T>>*>(a.ptr);
    if (!p) {
        throw std::bad_any_cast;
    }

    return p->value;
}

int main() {

    std::any a = 5;
    std::cout << std::any_cast<int>(a) << '\n';

    a = "abs";
    std::cout << std::any_cast<const char*>(a) << '\n';

    std::vector<int> v{1, 2, 3};

    a = v;
    std::cout << std::any_cast<std::vector<int>&>(a)[0] << '\n';
}