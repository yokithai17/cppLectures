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

#include <string>
int main() {
    /*
    std::vector<int> v;
    for (int i = 0; i < 50; ++i) {
        v.push_back(i);
    }

    int* p = new int[100];
    delete[] p;
    */
   std::string* p = new std::string[2];
   std::cout << p << '\n';
   --p;
   std::cout << *reinterpret_cast<size_t*>(p) << '\n';
   ++p;
   delete[] p;
}