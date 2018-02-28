#pragma once

#include "weak_hash_table_base.h"
#include "weak_traits.h"

/// Namespace for weak pairs and hash tables.
namespace weak {

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

template <class Key, class Hash, class KeyEqual, class Allocator>
void swap(weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
          weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

template <class Key, class Hash, class KeyEqual, class Allocator>
bool subset(const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
            const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    for (const auto& elem : a)
        if (!b.member(*elem)) return false;

    return true;
}

template <class Key, class Hash, class KeyEqual, class Allocator>
bool operator==(const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
                const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    return subset(a, b) && subset(b, a);
}

template <class Key, class Hash, class KeyEqual, class Allocator>
bool operator!=(const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
                const weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    return !(a == b);
}

} // end namespace weak
