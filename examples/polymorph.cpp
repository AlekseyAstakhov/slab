#include "../slab.h"

#include <variant>
#include <iostream>

using namespace std;

int main()
{
    // If need to store polymorphic objects in the slab,
    // to avoid allocations you can use
    // static polymorphism instead of dinamic.
    // For example, polymorphism with std::variant and std::visit.

    struct Cat {
        const char* is() { return "Cat"; }
    };

    struct Dog {
        const char* is() { return "Dog"; }
    };

    using Animal = variant<Cat, Dog>;

    Slab<Animal> animals = { Dog(), Cat(), Dog(), Cat(), Cat() };

    for (auto &animal : animals) {
        visit([] (auto &animal) {
            cout << animal.is() << endl;
        } , animal);
    }

    return 0;
}
