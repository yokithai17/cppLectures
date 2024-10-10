#include <iostream>

// XV Unions and type aliasing

// 15.1 Unions and their usage

union U {
    int x;
    double y;
};

int main() {
    U u;
    U.x = 5;
}
