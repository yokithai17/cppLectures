#include <iostream>
#include <vector>
// 9.4. Operators new and delete overloading.

void* operator new(size_t n) {
    std::cout << n << " bytes allocated\n";
    return malloc(n);
}

void operator delete(void* ptr) {
    free(ptr);
}

void* operator new[](size_t n) {
    std::cout << n << "[] bytes allocated\n";
    return malloc(n);
}

void operator delete[](void* ptr) {
    free(ptr);
}

void* operator new(size_t n, int a, double b) {
    std::cout << n << " bytes allocated with custom new " << a << ' ' << b << '\n';
    return malloc(n); 
}

void operator delete(void* ptr, int a, double b) {
    std::cout << "custom delete called " << a << ' ' << b << '\n';
    free(ptr);
}

struct S {
    inline static int count = 0;
    S() {
        ++count;
        if(count == 5) {
            throw 1;
        }
        std::cout << "Created!\n";
    }

    ~S() {
        std::cout << "destroyed!\n";
    }
};

struct Base {
    Base() { std::cout << "created Base\n"; }
    ~Base() { std::cout << "destroyed Base\n"; }
};

struct Derived : Base {
    Derived() { std::cout << "created Derived\n"; }
    ~Derived() { std::cout << "destroyed Derivedn\n"; }
};

#include <string>
int main() {
    /*
    std::vector<int> v;
    for (int i = 0; i < 50; ++i) {
        v.push_back(i);
    }

    int* p = new int[100];
    delete[] p;
    
   delete[] p;


    try {
        S* p = new S[10];
    }
    catch (...) {
        std::cout << "caught!";
    }    
    */
}