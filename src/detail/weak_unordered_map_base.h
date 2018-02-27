#pragma once

#include "weak_hash_table_base.h"

namespace weak::detail {

template<
        class KeyValue,
        class Hash,
        class KeyEqual,
        class Allocator
>
class weak_unordered_map_base
        : public weak_hash_table_base<KeyValue, Hash, KeyEqual, Allocator>
{
    using BaseClass = weak_hash_table_base<KeyValue, Hash, KeyEqual, Allocator>;
public:
    using BaseClass::weak_hash_table_base;
};

} // end namespace weak::detail
