#pragma once

#include "rh_weak_hash_table.h"

namespace weak::detail {

template<
        class KeyValue,
        class Hash,
        class KeyEqual,
        class Allocator
>
class weak_unordered_map_base
        : public rh_weak_hash_table<KeyValue, Hash, KeyEqual, Allocator>
{
    using BaseClass = rh_weak_hash_table<KeyValue, Hash, KeyEqual, Allocator>;
public:
    using BaseClass::rh_weak_hash_table;
};

} // end namespace weak::detail
