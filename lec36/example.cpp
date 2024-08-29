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