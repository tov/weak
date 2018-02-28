#pragma once

#include "weak_hash_table_base.h"
#include "weak_traits.h"
#include "weak_key_pair.h"

#include <functional>

namespace weak {

/// A map whose keys are stored by `std::weak_ptr`s.
///
/// When a pointer to a key expires, that association of the map is lazily
/// removed.
template<class Key, class Value,
         class Hash = std::hash<Key>,
         class KeyEqual = std::equal_to<Key>,
         class Allocator = std::allocator<weak_key_pair<Key, Value>>>
class weak_key_unordered_map
        : public weak_hash_table_base<weak_key_pair<Key, Value>, Hash, KeyEqual, Allocator>
{
    using BaseClass = weak_hash_table_base<weak_key_pair<Key, Value>, Hash, KeyEqual, Allocator>;
    using typename BaseClass::Bucket;
public:
    using BaseClass::weak_hash_table_base;

    /// Looks up the given key in the hash table, returning a reference to
    /// the value.
    ///
    /// If the key doesn't exist then it is inserted and the value default
    /// constructed.
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

/// Swaps two `weak_key_unordered_map`s in constant time.
template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void swap(weak_key_unordered_map <Key, Value, Hash, KeyEqual, Allocator>& a,
          weak_key_unordered_map <Key, Value, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

/// Is `a` a submap of `b`?
///
/// That is, are all the keys of `a` keys of `b`, and all the values equal
/// according to `compare`. Function `compare` defaults to equality, but
/// other relations are possible.
template <class Key, class Value, class Hash, class KeyEqual, class Allocator,
          class ValueEqual = std::equal_to<Value>>
bool submap(
        const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
        const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b,
        ValueEqual compare = std::equal_to<Value>())
{
    for (const auto& elem : a) {
        auto iter = b.find(*elem.first);
        if (iter == b.end() || !compare((*iter).second, elem.second))
            return false;
    }

    return true;
}

/// Are the keys of `a` a subset of the keys of `b`?
template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool keys_subset(
        const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
        const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    submap(a, b, [](const auto&, const auto&) { return true; });
}

/// Are the given maps equal?
template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool operator==(const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
                const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    return submap(a, b) && keys_subset(b, a);
}

/// Are the given maps unequal?
template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool operator!=(const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
                const weak_key_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    return !(a == b);
}

} // end namespace weak
