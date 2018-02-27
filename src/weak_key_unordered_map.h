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
    using typename BaseClass::Bucket;
public:
    using BaseClass::weak_unordered_map_base;

    Value& operator[](const std::shared_ptr<const Key>& key)
    {
        Value* result;

        BaseClass::insert_helper_(
                *key,
                [&](Bucket& bucket) {
                    BaseClass::construct_bucket_(bucket, key, Value());
                    result = &bucket.value().second;
                },
                [&](Bucket& bucket) {
                    bucket.value().first = key;
                    bucket.value().second = Value();
                    result = &bucket.value().second;
                },
                [&](Bucket& bucket) {
                    bucket.value().first = key;
                    result = &bucket.value().second;
                });

        return *result;
    }
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void swap(weak_key_unordered_map <Key, Value, Hash, KeyEqual, Allocator>& a,
          weak_key_unordered_map <Key, Value, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

} // end namespace weak
