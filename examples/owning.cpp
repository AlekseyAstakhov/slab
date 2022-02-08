#include <iostream>
#include <chrono>
#include <list>
#include "../slab.h"

using namespace std;

struct TestType {
    bool moved = false;

    TestType() { cout << "constructor " << this << endl; }
    TestType(const TestType &) { cout << "copy constructor " << this << endl; }
    TestType(TestType &&) { moved = true; cout << "move construct " << this << endl; }
    ~TestType() { cout << "destruct " << this << " " << (moved ? "created with move" : "moved") << endl; }

    TestType & operator=(const TestType &) { cout << "operator=" << endl; return *this; }
};

int main() {
    // When adding object to the slab ownership is transferred to slab
    // with move or copy if moving don't posible.
    // The object will be destroyed when it is removed with function `Slab::remove`
    // or when the slab itself is deleted.
    // You can also get ownership of the object from the slab to yourself back
    // with method `take`.

    cout << "Create slab:" << endl;
    Slab<TestType> slab;
    cout << endl;

    cout << "Insert to slab:" << endl;
    size_t key = slab.insert(move(TestType()));
    cout << endl;

    cout << "Remove from slab:" << endl;
    slab.remove(key);
    //    optional<TestType> t = slab.take_and_remove(key);
    cout << endl;

    cout << "Finish!" << endl;
    return 0;
}
