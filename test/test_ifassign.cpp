#include <iostream>

int main() {
    int x = 10;

    if (int y = x * 2; y > 15) {
        std::cout << "y is greater than 15: " << y << std::endl;
    } else {
        std::cout << "y is not greater than 15: " << y << std::endl;
    }

    // y is not accessible outside the if-else block
    // std::cout << y << std::endl; // This will cause an error 

    return 0;
}
