#ifndef SLAB_H
#define SLAB_H

#include <functional>
#include <optional>
#include <initializer_list>
#include <vector>
#include <ostream>

/// Container with slab allocator logic.
/// Allows fast insert, look-up and remove elements. Avoids allocations.
/// https://en.wikipedia.org/wiki/Slab_allocation
template <class T>
class Slab {
    /// Slots of elements.
    std::vector<std::optional<T>> slots_pool;
    /// Stack of removed elements slots keys for reusing them for next inserted elements.
    std::vector<size_t> stack_of_removed;

public:
    /// Constructs a new empty slab container with zero capacity.
    constexpr Slab() {}

    /// Constructs a new slab container with specified reserved capacity.
    /// For stack of removed elements will set as capacity / 2.
    constexpr explicit Slab(size_t start_capacity) {
        slots_pool.reserve(start_capacity);
        stack_of_removed.reserve(start_capacity / 2);
    }

    /// Constructs a new slab container with values from initializer_list.
    /// Usually not needed when using slab, since constructor can't return keys.
    constexpr Slab(std::initializer_list<T> init) {
        slots_pool.reserve(init.size());
        for (const T &val : init) {
            slots_pool.push_back(val);
        }
    }

    /// Inserts a object and return the key of it in the slab.
    /// It should be noted that after you remove element from slab, key will be reused for new elements.
    /// Сomplexity O(1), but if not enough capacity will relocating memory and copying all elements same logic as std::vector in no capacity case.
    constexpr size_t insert(T &&obj) {
        size_t key = 0;
        if (stack_of_removed.empty()) {
            slots_pool.emplace_back(std::move(obj));
            key = slots_pool.size() - 1;
        } else {
            key = stack_of_removed.back();
            slots_pool[key].emplace(std::move(obj));
            stack_of_removed.pop_back();
        }

        return key;
    }

    /// Inserts a object and return the key of it in slab.
    /// It should be noted that after you remove element from slab, key will be reused for new elements.
    /// Сomplexity O(1), but if not enough capacity will relocating memory and copying all elements same logic as std::vector in no capacity case.
    constexpr size_t insert(T &obj) {
        size_t key = 0;
        if (stack_of_removed.empty()) {
            slots_pool.emplace_back(obj);
            key = slots_pool.size() - 1;
        } else {
            key = stack_of_removed.back();
            slots_pool[key].emplace(obj);
            stack_of_removed.pop_back();
        }
        return key;
    }

    /// Returns true if the object by the key exist or false if it doesn't.
    inline bool contains(size_t key) {
        return key < slots_pool.size() && slots_pool[key] != std::nullopt;
    }

    /// Returns a reference to the object in the slab by the key.
    /// Does not check whether the slab contains a value.
    /// If the object by key doesn't exist then undefined behavior.
    /// To check for the existence use contains().
    /// Сomplexity O(1).
    inline T& get(size_t key) {
        return *slots_pool[key];
    }

    /// Returns a const reference to the object in the slab by the key.
    /// Does not check whether the slab contains a value.
    /// If the object by key doesn't exist then undefined behavior.
    /// To check for the existence use contains().
    /// Сomplexity O(1).
    inline const T& get(size_t key) const {
        return *slots_pool[key];
    }

    /// Removes object from the slab by the key.
    /// Returns false if obect by key not exist.
    /// Сomplexity O(1).
    bool remove(size_t key) {
        if (key >= slots_pool.size())
            return false;

        auto &obj = slots_pool[key];
        if (slots_pool[key] == std::nullopt)
            return false;

        obj = std::nullopt;
        stack_of_removed.push_back(key);

        return true;
    }

    /// Returns determined the slab key what will assigned for next added object.
    /// Сomplexity O(1).
    inline size_t vacant_key() const {
        return stack_of_removed.empty() ? slots_pool.size() : stack_of_removed.back();
    }

    /// Move object from the slab by the key.
    /// Returns moved stored object or std::nullopt if obect by key not exist.
    /// Сomplexity O(1).
    inline std::optional<T> take(size_t key) {
        std::optional<T> res = std::nullopt;
        if (key >= slots_pool.size())
            return res;

        slots_pool[key].swap(res);
        if (res != std::nullopt)
            stack_of_removed.push_back(key);

        return res;
    }

    /// Returns the number of stored objects.
    /// Сomplexity O(1).
    inline size_t size() const {
        return slots_pool.size() - stack_of_removed.size();
    }

    /// Returns true if there are no objects stored in the slab.
    /// Сomplexity O(1).
    inline bool empty() const {
        return size() == 0;
    }

    /// Returns the number of objects the slab can store without reallocating.
    inline size_t slots_capacity() const {
        return slots_pool.capacity();
    }

    /// Returns the capacity of the removed objects stack.
    inline size_t stack_capacity() const {
        return stack_of_removed.capacity();
    }

    /// Slab iterator.
    ///
    /// Iterator is bidirectional.
    /// Allows use it with standard algorithms.
    /// It should be noted that iteration from one elements to another
    /// may include iterations of skipping removed objects slots.
    /// Usually even if there are many removed elements in the slab,
    /// iterations is fast, like a iterations of the std::vector.
    class Iterator
    {
        Iterator(Slab<T> &slab, size_t p) : slab(slab), pos(p) {
            while (pos < slab.slots_pool.size() && slab.slots_pool[pos] == std::nullopt)
                ++pos;
        }

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::size_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        friend Slab;

        inline Iterator & operator=(const Iterator &right) {
            pos = right.pos;
            return *this;
        }

        inline reference operator*() const {
            return *slab.slots_pool[pos];
        }

        inline pointer operator->() {
            return &*slab.slots_pool[pos];
        }

        inline Iterator & operator++() {
            // skeep empty slots
            while (++pos < slab.slots_pool.size() && slab.slots_pool[pos] == std::nullopt);
            return *this;
        }

        inline Iterator & operator--() {
            int s = slab.slots_pool.size();
            // skeep empty slots
            while (--pos > 0 && slab.slots_pool[pos] == std::nullopt);
            return *this;
        }

        friend inline bool operator==(const Iterator &a, const Iterator &b) {
            return a.pos == b.pos;
        }

        friend inline bool operator!=(const Iterator &a, const Iterator &b) {
            return a.pos != b.pos;
        }

        inline bool operator<(const Iterator &other) {
            return pos < other.pos;
        }

        inline difference_type operator-(const Iterator &right) {
            return pos - right.pos;
        }

        inline Iterator operator+(const int &right) {
            return Iterator(slab, pos + right);
        }

        inline Iterator operator-(const int &right) {
            return Iterator(slab, pos - right);
        }

    protected:
        Slab<T> &slab;
        size_t pos;
    };

    /// Returns bidirectional iterator to the beginning.
    inline Iterator begin() { return Iterator(*this, 0); }

    /// Returns bidirectional iterator to the end (past-the-last element).
    inline Iterator end() { return Iterator(*this, slots_pool.size()); }


    /// Slab iterator where dereferencing presented as key value pair.
    ///
    /// Iterator is forward.
    /// Allows iterate via slab collection and use only minimal set of standard algorithms.
    /// It should be noted that iteration from one elements to another
    /// may include iterations of skipping removed objects slots.
    /// Usually even if there are many removed elements in the slab,
    /// iterations is fast, like a iterations of the std::vector.

    class KeyValIterator : public Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::size_t;
        using value_type        = std::pair<size_t, T>;
        using pointer           = std::pair<size_t, T*>;
        using reference         = std::pair<size_t, T&>;
        friend Slab;

        KeyValIterator(Slab<T> &slab, size_t pos)
            : Iterator(slab, pos) {
        }

    public:
        inline reference operator*() const {
            return std::pair<size_t, T&>(Iterator::pos, *(Iterator::slab.slots_pool[Iterator::pos]));
        }

        inline pointer operator->() {
            return std::pair(Iterator::pos, &*Iterator::slab.slots_pool[Iterator::pos]);
        }
    };

    /// Returns forward iterator to the beginning.
    /// Dereferencing this iterator presented as key value pair.
    inline KeyValIterator key_val_begin() { return KeyValIterator(*this, 0); }

    /// Returns forward iterator to the end (past-the-last element).
    /// Dereferencing this iterator presented as key value pair.
    inline KeyValIterator key_val_end() { return KeyValIterator(*this, slots_pool.size()); }

    /// For out all slab elements to std::ostream with custom separator.
    /// Type of elements must have operator << .
    std::ostream& out(std::ostream& stream, const char &separator = ' ')
    {
        int i = 0;
        for (const T &obj : *this) {
            stream << obj;
            if (i < size() - 1)
                stream << separator;
            ++i;
        }
        return stream;
    }
};

/// Operator << for out to ostream all elements of slab collection.
/// Elements separated by a space. Type of elements must have operator << .
template <class T>
std::ostream& operator<<(std::ostream& stream, Slab<T> &slab) {
    return slab.out(stream);
}

#endif
