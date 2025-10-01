#include <iostream>
#include <string>

namespace mylib {
    __declspec(dllexport) void hello() {
        std::cout << "Hello, World!" << std::endl;
    }

    __declspec(dllexport) int add(int a, int b) {
        return a + b;
    }
}

int main() {
    mylib::hello();
    std::cout << "3 + 4 = " << mylib::add(3, 4) << std::endl;
    return 0;
}