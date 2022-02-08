/// Runs tests and return 0 if successful.
/// Return 1 if some test fail.
/// In fail case cout the function name and the line number
/// of the place where was used the `FAIL` macro.
///
/// The function `main` at the end of the file.

#include "../slab.h"
#include <iostream>
#include <chrono>
#include <deque>
#include <initializer_list>
#include <debug/debug.h>

/// For easy debugging. By using FAIL macros will go to here
void fail_end() {
    exit(1); // place for breakpoint
}

/// Just std::cout the text with the function name where used this macro.
#define TEST {cout << "start test " << __FUNCTION__ << std::endl;}
/// std::cout text with the function name and the line where was used this macro. Makes the exit(1).
#define FAIL {cout << "Fail in "<< __FUNCTION__ << ", file: "<< __FILE__ << ", line: " << __LINE__ << std::endl; fail_end();}

using namespace std;
using namespace chrono;

void create() {
    TEST

    Slab<int> slab;
    if (!slab.empty())
        FAIL
}

/// Struct for testing copy, move and assignment behavior.
struct TestStruct {
    volatile int *copy_cnt;
    volatile int *move_cnt;
    volatile int *assignment_cnt;
    volatile int *assignment_move_cnt;

    TestStruct(int *construct_cnt, int *copy_cnt, int *move_cnt, int *assignment_cnt, int *assignment_move_cnt)
        : copy_cnt(copy_cnt)
        ,  move_cnt(move_cnt)
        ,  assignment_cnt(assignment_cnt)
        ,  assignment_move_cnt(assignment_move_cnt)
    {
        ++(*construct_cnt);
    }

    TestStruct(const TestStruct &other) {
        copy_cnt = other.copy_cnt;
        move_cnt = other.move_cnt;
        assignment_cnt = other.assignment_cnt;
        assignment_move_cnt = other.assignment_move_cnt;
        ++(*copy_cnt);
    }

    TestStruct(const TestStruct &&other) {
        copy_cnt = other.copy_cnt;
        move_cnt = other.move_cnt;
        assignment_cnt = other.assignment_cnt;
        assignment_move_cnt = other.assignment_move_cnt;
        ++(*move_cnt);
    }

    TestStruct& operator=(const TestStruct &other) {
        copy_cnt = other.copy_cnt;
        move_cnt = other.move_cnt;
        assignment_cnt = other.assignment_cnt;
        assignment_move_cnt = other.assignment_move_cnt;
        ++(*assignment_cnt);
        return *this;
    }

    TestStruct& operator=(const TestStruct &&other) {
        copy_cnt = other.copy_cnt;
        move_cnt = other.move_cnt;
        assignment_cnt = other.assignment_cnt;
        assignment_move_cnt = other.assignment_move_cnt;
        ++(*assignment_move_cnt);
        return *this;
    }
};

void insert() {
    TEST

    Slab<int> slab;
    slab.insert(1);
    if (slab.empty() || slab.size() != 1)
        FAIL

    slab.insert(2);
    if (slab.empty() || slab.size() != 2)
        FAIL

    auto key_of_3 = slab.insert(3);
    if (slab.empty() || slab.size() != 3)
        FAIL

    slab.insert(4);
    if (slab.empty() || slab.size() != 4)
        FAIL

    if (!slab.remove(key_of_3))
        FAIL

    if (slab.empty() || slab.size() != 3)
        FAIL

    slab.insert(5);
    if (slab.empty() || slab.size() != 4)
        FAIL

    if(!equal(slab.begin(), slab.end(), vector { 1, 2, 5, 4 }.begin()))
        FAIL

    {
        Slab<TestStruct> slab;
        int construct_cnt = 0; int copy_cnt = 0; int move_cnt = 0;
        int assignment_cnt = 0; int assignment_move_cnt = 0;
        TestStruct a(&construct_cnt, &copy_cnt, &move_cnt, &assignment_cnt, &assignment_move_cnt);
        slab.insert(std::move(a));
        if (construct_cnt != 1 || copy_cnt != 0 || move_cnt != 1 || assignment_cnt != 0 || assignment_move_cnt != 0)
            FAIL
    }

    {
        Slab<TestStruct> slab;
        int construct_cnt = 0; int copy_cnt = 0; int move_cnt = 0;
        int assignment_cnt = 0; int assignment_move_cnt = 0;
        TestStruct a(&construct_cnt, &copy_cnt, &move_cnt, &assignment_cnt, &assignment_move_cnt);
        slab.insert(a);
        if (construct_cnt != 1 || copy_cnt != 1 || move_cnt != 0 || assignment_cnt != 0 || assignment_move_cnt != 0)
            FAIL
    }

    {
        Slab<TestStruct> slab;
        int construct_cnt = 0; int copy_cnt = 0; int move_cnt = 0;
        int assignment_cnt = 0; int assignment_move_cnt = 0;
        slab.insert(TestStruct(&construct_cnt, &copy_cnt, &move_cnt, &assignment_cnt, &assignment_move_cnt));
        if (construct_cnt != 1 || copy_cnt != 0 || move_cnt != 1 || assignment_cnt != 0 || assignment_move_cnt != 0)
            FAIL
    }
}

void get() {
    TEST

    Slab<int> slab;
    auto key0 = slab.insert(0);
    auto key1 = slab.insert(1);
    auto key2 = slab.insert(2);
    auto key3 = slab.insert(3);
    auto key4 = slab.insert(4);

    if(!equal(slab.begin(), slab.end(), vector { 0, 1, 2, 3, 4 }.begin()))
        FAIL

    if(!slab.contains(key0) || !slab.contains(key1) || !slab.contains(key2) || !slab.contains(key3) || !slab.contains(key4))
        FAIL

    if(slab.get(key0) != 0 || slab.get(key1) != 1 || slab.get(key2) != 2 || slab.get(key3) != 3 || slab.get(key4) != 4)
        FAIL

    if(!slab.remove(key2))
        FAIL

    if(slab.contains(key2))
        FAIL

    if(!slab.contains(key0) || !slab.contains(key1) || !slab.contains(key3) || !slab.contains(key4))
        FAIL

      if(slab.contains(2134124124))
         FAIL

    {
        Slab<TestStruct> slab;
        int construct_cnt = 0; int copy_cnt = 0; int move_cnt = 0;
        int assignment_cnt = 0; int assignment_move_cnt = 0;
        TestStruct a(&construct_cnt, &copy_cnt, &move_cnt, &assignment_cnt, &assignment_move_cnt);
        auto key = slab.insert(std::move(a));
        slab.get(key);
        if (copy_cnt != 0 || move_cnt != 1 || assignment_cnt != 0 || assignment_move_cnt != 0)
            FAIL
    }
}

void remove() {
    TEST

    Slab<int> slab;
    auto key0 = slab.insert(0);
    auto key1 = slab.insert(1);
    auto key2 = slab.insert(2);

    if(!equal(slab.begin(), slab.end(), vector { 0, 1, 2 }.begin()))
        FAIL

    slab.remove(key1);
    if(!equal(slab.begin(), slab.end(), vector { 0, 2 }.begin()))
        FAIL

    slab.remove(key2);
    if(!equal(slab.begin(), slab.end(), vector { 0 }.begin()))
        FAIL

    slab.remove(key0);
    if(!slab.empty())
        FAIL
}

void take() {
    TEST

    Slab<int> slab;
    auto key0 = slab.insert(0);
    auto key1 = slab.insert(1);
    auto key2 = slab.insert(2);
    auto key3 = slab.insert(3);
    auto key4 = slab.insert(4);

    optional<int> val1 = slab.take(key1);
    if (!val1 && *val1 != 1)
        FAIL

    if (slab.contains(key1) || slab.size() != 4)
        FAIL

    optional<int> val4 = slab.take(key4);
    if (!val1 && *val4 != 4)
        FAIL

    if (slab.contains(key1) || slab.size() != 3)
        FAIL

    if (slab.contains(key4))
        FAIL

    if (!slab.remove(key3))
        FAIL

    if (slab.contains(key1) || slab.size() != 2)
        FAIL

    optional<int> val3 = slab.take(key3);
    if (val3)
        FAIL

    if (slab.contains(key1) || slab.size() != 2)
        FAIL

    optional<int> val2 = slab.take(key2);
    if (!val2 && *val2 != 2)
        FAIL

    if (slab.size() != 1)
        FAIL

    optional<int> val0 = slab.take(key0);
    if (!val0 && *val0 != 0)
        FAIL

    if (!slab.empty())
        FAIL

    {
        Slab<TestStruct> slab;
        int construct_cnt = 0; int copy_cnt = 0; int move_cnt = 0;
        int assignment_cnt = 0; int assignment_move_cnt = 0;
        TestStruct a(&construct_cnt, &copy_cnt, &move_cnt, &assignment_cnt, &assignment_move_cnt);
        auto key = slab.insert(std::move(a));
        if (!slab.take(key))
            FAIL

        if (copy_cnt != 0 || move_cnt != 2 || assignment_cnt != 0 || assignment_move_cnt != 0)
            FAIL
    }
}

void vacant_key() {
    TEST

    Slab<int> slab;
    auto key0 = slab.insert(0);
    auto key1 = slab.insert(1);
    auto key2 = slab.insert(2);

    if (slab.vacant_key() != 3)
        FAIL

    slab.remove(key1);

    if (slab.vacant_key() != 1)
        FAIL

    slab.remove(key2);

    if (slab.vacant_key() != 2)
        FAIL

    auto key3 = slab.insert(3);
    if (slab.vacant_key() != 1)
        FAIL

    if (slab.vacant_key() != 1)
        FAIL

    auto key4 = slab.insert(4);
    if (slab.vacant_key() != 3)
        FAIL

    auto key5 = slab.insert(5);
    if (slab.vacant_key() != 4)
        FAIL

    slab.remove(key0);

    if (slab.vacant_key() != 0)
        FAIL

    auto key6 = slab.insert(6);
    if (slab.vacant_key() != 4)
        FAIL

    if(!equal(slab.begin(), slab.end(), vector { 6, 4, 3, 5 }.begin()))
        FAIL

    slab.remove(key3);
    if (slab.vacant_key() != 2)
        FAIL

    slab.remove(key4);
    if (slab.vacant_key() != 1)
        FAIL

    slab.take(key6);
    if (slab.vacant_key() != 0)
        FAIL

    slab.take(key5);
    if (slab.vacant_key() != 3)
        FAIL

    if (!slab.empty())
        FAIL
}

void capacity() {
    TEST

    Slab<int> slab;
    if (slab.slots_capacity() != 0 || slab.stack_capacity() != 0)
        FAIL;

    auto create_with_capacity = [&](size_t capacity) {
        slab = Slab<int>(capacity);
        vector<size_t> keys;

        for (int i = 0; i < capacity; ++i) {
            size_t key = slab.insert(0);
            keys.push_back(key);
        }

        if (slab.slots_capacity() != capacity || slab.stack_capacity() != capacity / 2)
            FAIL;

        size_t last_key = slab.insert(0);
        if (slab.slots_capacity() <= capacity || slab.stack_capacity() != capacity / 2)
            FAIL;

        size_t slots_capacity = slab.slots_capacity();
        size_t stack_capacity = slab.stack_capacity();

        for (int i = 0; i < keys.size() / 2; ++i) {
            slab.remove(keys[i]);
        }

        if (slab.stack_capacity() != stack_capacity)
            FAIL;

        slab.remove(last_key);

        if (slab.stack_capacity() <= stack_capacity)
            FAIL;
    };

    for (int i = 0; i < 1024; ++i) {
        create_with_capacity(0);
    }
    create_with_capacity(12351478);
}

template <class T>
inline void create_initialize_and_test_equal(initializer_list<T> init) {
    Slab<T> slab(init);
    if (!equal(slab.begin(), slab.end(), init.begin()) || slab.size() != init.size())
        FAIL
}

void initializer_lists() {
    TEST

    initializer_list<int> int_list;

    create_initialize_and_test_equal({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
    create_initialize_and_test_equal({ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 });
    create_initialize_and_test_equal({ "abcd", "slab", "shgd", "1!@#" });

    struct A {
    };
    {
        Slab<A> slab { A(), A(), A(), A(), A() };
        if (slab.size() != 5)
            FAIL
    }

    struct B {
        int x;
        B(int x) : x(x) {}
        bool operator==(const B &other) { return x == other.x; }
    };
    create_initialize_and_test_equal<B>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

    struct C {
        int x;
        C(int&& x) : x(std::move(x)) {}
        bool operator==(const C &other) { return x == other.x; }
    };
    create_initialize_and_test_equal<C>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

    struct D {
        D() {}
        D(const D &) {}
        D(D &&) {}
        ~D() {}
        D & operator=(const D &) { return *this; }
    };
    {
        Slab<D> slab = { D(), D(), D() };
        if (slab.size() != 3)
            FAIL
    }
}

void iterators() {
    TEST

    Slab<int> slab;
    slab.insert(0);
    slab.insert(1);
    slab.insert(2);
    slab.insert(3);
    slab.insert(4);

    if(!equal(slab.begin(), slab.end(), vector { 0, 1, 2, 3, 4 }.begin()))
        FAIL

    reverse(slab.begin(), slab.end());

    if(!equal(slab.begin(), slab.end(), vector { 4, 3, 2, 1, 0 }.begin()))
        FAIL

    auto it = slab.begin();
    ++it;
    if (*it != 3)
        FAIL

    ++it;
    if (*it != 2)
        FAIL

    *it = 1;

    ++it;
    if (*it != 1)
        FAIL

    *it = 2;

    if (find(slab.begin(), slab.end(), 2) != it)
        FAIL

    ++it;
    if (it == slab.end())
        FAIL

    ++it;
    if (it != slab.end())
        FAIL

    --it;
    if (it == slab.end())
        FAIL

    if (*it != 0)
        FAIL

    --it;
    --it;
    if (*it != 1)
        FAIL

    --it;
    --it;

    if (*it != 4)
        FAIL

    if (it != slab.begin())
        FAIL

    if(!equal(slab.begin(), slab.end(), vector { 4, 3, 1, 2, 0 }.begin()))
        FAIL

    sort(slab.begin(), slab.end());

    if(!equal(slab.begin(), slab.end(), vector { 0, 1, 2, 3, 4 }.begin()))
        FAIL

     struct A {
         int x;
     };

     Slab<A> slab2;
     slab2.insert(A{0});
     slab2.begin()->x = 3;
     if ((*slab2.begin()).x != 3)
         FAIL
}

void bench() {
    TEST

    size_t n = 100000000;

    // insert to allocated vec and slab

    auto start = steady_clock::now();
    Slab<int> slab(n);
    for (int i = 0; i < n; ++i) {
        slab.insert(std::move(i));
    }
    auto slab_elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    cout << "insert to alocated slab: " << slab_elapsed << " mills" << endl;

    start = steady_clock::now();
    vector<int> vec(n);
    for (int i = 0; i < n; ++i) {
        vec[i] = i;
    }
    auto vec_elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    cout << "insert to alocated vector: " << vec_elapsed << " mills" << endl;

    // find element in middle

    int find_val = n / 2 + n / 4;

    start = steady_clock::now();
    find_if(slab.begin(), slab.end(), [&](int x) { return x == find_val; });
    auto slab_elapsed_nanos = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "find element in middle slab: " << slab_elapsed_nanos << " nanos" << endl;

    start = steady_clock::now();
    find_if(slab.begin(), slab.end(), [&](int x) { return x == find_val; });
    auto vec_elapsed_nanos = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "find element in middle vec: " << vec_elapsed_nanos << " nanos" << endl;

    // remove each 2 element

    for (int i = 0; i < n; ++i) {
        auto o = slab.get(i);
        if (o && (o % 2) == 0) {
            slab.remove(i);
        }
    }

    vec.erase(remove_if(vec.begin(), vec.end(), [&](int x) { return x % 2 == 0; }), vec.end());

    // remove element from middle

    start = steady_clock::now();
    slab.remove(n / 2);
    slab_elapsed = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "remove element from middle slab: " << slab_elapsed << " nanos" << endl;

    start = steady_clock::now();
    vec.erase(vec.begin() + vec.size() / 2);
    vec_elapsed = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "remove element from middle vec: " << vec_elapsed << " nanos" << endl;

    if (slab_elapsed > vec_elapsed / 10000)
        FAIL

    // find last element

    start = steady_clock::now();
    find_if(slab.begin(), slab.end(), [&](int x) { return x == find_val; });
    slab_elapsed_nanos = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "find last in slab: " << slab_elapsed_nanos << " nanos" << endl;

    start = steady_clock::now();
    find_if(vec.begin(), vec.end(), [&](int x) { return x == find_val; });
    vec_elapsed_nanos = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "find last in vec: " << vec_elapsed_nanos << " nanos" << endl;

    // insert one

    start = steady_clock::now();
    slab.insert(2);
    slab_elapsed_nanos = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "insert one to slab: " << slab_elapsed_nanos << " nanos" << endl;

    start = steady_clock::now();
    vec.push_back(2);
    vec_elapsed_nanos = duration_cast<nanoseconds>(steady_clock::now() - start).count();
    cout << "insert one to vec: " << vec_elapsed_nanos << " nanos" << endl;
}

/// Run tests and returns 0 is successful. Return 1 if some test fail with macro FAIL was used.
/// In fail case std::cout function name and line number of place where was used FAIL macro.
int main() {
    create();
    insert();
    get();
    remove();
    take();
    vacant_key();
    capacity();
    initializer_lists();
    iterators();
//    bench();

    cout << "All tests are successful." << std::endl;
}
