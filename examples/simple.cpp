#include <iostream>
#include "../slab.h"

using namespace std;

int main()
{
    Slab<float> slab;
    size_t key1 = slab.insert(3.14);
    cout << slab.get(key1) << endl;
    slab.remove(key1);

    return 0;
}
