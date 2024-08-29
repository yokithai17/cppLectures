#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

template <typename T>
class istream_iterator {
private:
    std::istream* in = nullptr;
    T value;
public:
    istream_iterator() {}

    istream_iterator(std::istream& in): in(&in) {
        in >> value;
    }

    istream_iterator& operator++() {
        if (!(in >> value)) {
            *this = istream_iterator();
        }

        return *this;
    }

    T& operator*() {
        return value;
    }
};

template <typename T>
class optional {
private:
    char value[sizeof(T)];
    bool init = false;
public:
    optional() {}
    optional(const T& val): init(true) {
        new (value) T(val);
    }

    ~optional() {
        if (init) {
            reinterpret_cast<T*>(value)->~T();
        }
    }

    bool has_value() const {
        return init;
    }

    operator bool() const {
        return init;
    }

    T& operator*() {
        return reinterpret_cast<T&>(*value);
    }

    const T& value_or(const T& other) const {
        return init ? reinterpret_cast<T&>(*value) : other;
    }
};


int main() {
    std::istream_iterator<int> it(std::cin);
    std::vector<int> v;

    std::copy_n(it, 10, std::back_inserter(v));

    std::ifstream in("input.txt");
    std::istream_iterator<int> itt(in);
    // std::copy_if(itt, std::istream_iterator<int>()
    //    , std::ostream_iterator<int>(std::cout, " "));

    std::transform(itt, std::istream_iterator<int>(),
        std::ostream_iterator<int>(std::cout, " ")
        , [](int x) {return x * x;});
}