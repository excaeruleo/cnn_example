#include <iostream>

namespace Zoo {
    // Base class in the Zoo namespace
    class Animal {
    public:
        void eat() {
            std::cout << "This animal eats food." << std::endl;
        }
    };
}

// Derived class in the global namespace
class Dog : public Zoo::Animal { // Inheriting from Zoo::Animal
public:
    void bark() {
        std::cout << "The dog barks." << std::endl;
    }
};

int main() {
    Dog myDog;
    myDog.eat();  // Inherited from Zoo::Animal
    myDog.bark(); // Defined in Dog
    return 0;
}

