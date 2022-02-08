# slab

 Container with slab allocator logic.
 Allows fast insert, look-up and remove elements.
 Avoids allocations.

 Slab allocation is a memory management mechanism intended for
 the efficient memory allocation of objects.
 In comparison with earlier mechanisms,
 it reduces fragmentation caused by allocations and deallocations.
 This technique is used for retaining allocated memory containing
 a data object of a certain type for reuse upon subsequent
 allocations of objects of the same type. It is analogous to an object pool,
 but only applies to memory, not other resources.'

 Slab allocation renders infrequent the very costly practice (in CPU time)
 of initialization and destruction of data-objects, which can outweigh
 the cost of allocating memory for them. When the creates
 and deletes objects often, overhead costs of initialization can result
 in significant performance drops. Object caching leads to less frequent
 invocation of functions which initialize object state: when a slab-allocated
 object is released after use, the slab allocation system typically
 keeps it cached (rather than doing the work of destroying it)
 ready for re-use next time an object of that type is needed
 (thus avoiding the work of constructing and initialising a new object).

 Slab allocation was first introduced in the Solaris 2.4 kernel by Jeff Bonwick.
 It is now widely used by many Unix and Unix-like operating systems including
 FreeBSD and Linux.

 https://en.wikipedia.org/wiki/Slab_allocation

 This implementation is made using two std::vectors, where the first one is element slots
 and the second is the stack of references to the slots of removed elements. It is simple, 
 reliable, efficient and has intuitively predictable behavior.

### Usage
```c++
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
```

### License

Licensed under either of
* Apache License, Version 2.0 (LICENSE-APACHE or http://www.apache.org/licenses/LICENSE-2.0)
* MIT license (LICENSE-MIT or http://opensource.org/licenses/MIT) at your option.

at your option.
