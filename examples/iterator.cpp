#include "../slab.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

int main()
{
    // Slab is usually not used in tasks where iteration of elements is necessary,
    // but it's possible.
    // When using iterators, be aware that iterations
    // may involve hidden skips of free slots of removed elements.
    // Often this is not a big problem for performance.

    // Some examples:

    Slab<int> nums {1, 2, 4, 8, 16};
    Slab<string> fruits {"orange", "apple", "raspberry"};
    Slab<char> empty;

    // Print all elements.
    for_each(nums.begin(), nums.end(), [](const int n) { cout << n << ' '; });
    cout << endl;

    // Sums all integers in the slab nums (if any), printing only the result.
    cout << "Sum of nums: "
              << accumulate(nums.begin(), nums.end(), 0) << endl;

    // Prints the first fruit in the slab fruits, checking if there is any.
    if (!fruits.empty())
        cout << "First fruit: " << *fruits.begin() << endl;

    if (empty.begin() == empty.end())
        cout << "slab 'empty' is indeed empty." << endl;
}
