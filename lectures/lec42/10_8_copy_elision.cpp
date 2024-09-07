#include <iostream>


// 10.8 Expired values, RVO, copy elision

struct S {
    int x = 0;
    S(int x) : x(x) {
        std::cout << "created" << std::endl;
    }
    
    S(S&& x): x(x.x) {
        std::cout << "MOVE" << std::endl;
    }

    ~S() {
        std::cout << "Destroyed" << std::endl;
    }

};

int main() {
    S s = S(0);
}