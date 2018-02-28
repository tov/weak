#pragma once

#include "weak_hash_table_base.h"
#include "weak_traits.h"

/// Namespace for weak pairs and hash tables.
namespace weak {

/// An unordered set of weak pointers.
///
/// Uses key equality, not pointer equality, even though the keys are held
/// behind pointers.
template <
    class Key,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>
>
class weak_unordered_set :
    public weak_hash_table_base<std::weak_ptr<const Key>,
                                Hash, KeyEqual, Allocator>
{
    using BaseClass = weak_hash_table_base<std::weak_ptr<const Key>,
                                           Hash, KeyEqual, Allocator>;
public:
    using BaseClass::weak_hash_table_base;
};

/// Swaps two `weak_unordered_set`s in constant time.
template <class Key, class Hash, class KeyEqual, class Allocator>
void swap(weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
          weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

/// Is `a` a subset of `b`?
template <class Key, class Hash, class KeyEqual, class Allocator>
bool subset(const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
            const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    for (const auto& elem : a)
        if (!b.member(*elem)) return false;

    return true;
}

/// Equality for `weak_unordered_set`s.
template <class Key, class Hash, class KeyEqual, class Allocator>
bool operator==(const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
                const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    return subset(a, b) && subset(b, a);
}

/// Disequality for `weak_unordered_set`s.
template <class Key, class Hash, class KeyEqual, class Allocator>
bool operator!=(const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
                const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    return !(a == b);
}

} // end namespace weak
