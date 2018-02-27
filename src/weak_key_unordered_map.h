#pragma once

#include "detail/weak_hash_table_base.h"
#include "weak_traits.h"
#include "weak_key_pair.h"

namespace weak {

template<class Key, class Value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<weak_key_pair<Key, Value>>>
class weak_key_unordered_map
        : public detail::weak_unordered_map_base<weak_key_pair<Key, Value>, Hash, KeyEqual, Allocator>
{
    using BaseClass = detail::weak_unordered_map_base<weak_key_pair<Key, Value>, Hash, KeyEqual, Allocator>;
public:
    using BaseClass::weak_unordered_map_base;
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void swap(weak_key_unordered_map <Key, Value, Hash, KeyEqual, Allocator>& a,
          weak_key_unordered_map <Key, Value, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

} // end namespace weak
