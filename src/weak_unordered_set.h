#pragma once

#include "detail/rh_weak_hash_table.h"
#include "weak_traits.h"

namespace weak {

template <
    class Key,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>
>
class weak_unordered_set :
    public detail::rh_weak_hash_table<std::weak_ptr<const Key>,
                                      Hash, KeyEqual, Allocator>
{
    using BaseClass = detail::rh_weak_hash_table<std::weak_ptr<const Key>,
                                                 Hash, KeyEqual, Allocator>;
public:
    using BaseClass::rh_weak_hash_table;
};

template <class Key, class Hash, class KeyEqual, class Allocator>
void swap(weak_unordered_set<Key, Hash, KeyEqual, Allocator>& a,
          weak_unordered_set<Key, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}


} // end namespace weak
