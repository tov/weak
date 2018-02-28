#pragma once

#include "weak_hash_table_base.h"
#include "weak_traits.h"
#include "weak_weak_pair.h"

#include <functional>

namespace weak {

template<
        class Key, class Value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<weak_weak_pair<Key, Value>>>
class weak_weak_unordered_map
        : public weak_hash_table_base<weak_weak_pair<Key, Value>,
                                              Hash, KeyEqual, Allocator>
{
    using BaseClass = weak_hash_table_base<weak_weak_pair<Key, Value>,
                                                   Hash, KeyEqual, Allocator>;
    using Bucket = typename BaseClass::Bucket;
public:
    using BaseClass::weak_hash_table_base;

    class proxy
    {
    public:
        proxy(const proxy&) noexcept = default;
        proxy(proxy&&) noexcept = default;

        proxy& operator=(const proxy&) = delete;
        proxy& operator=(proxy&&) = delete;

        Value& operator*() const noexcept
        {
            return *value_ptr_;
        }

        std::shared_ptr<Value> operator->() const noexcept
        {
            return value_ptr_;
        }

        operator std::shared_ptr<Value>() const noexcept
        {
            return value_ptr_;
        }

        proxy& operator=(const std::shared_ptr<Value>& value)
        {
            bucket_.value().second = value_ptr_ = value;
            return *this;
        }

        proxy& operator=(std::shared_ptr<Value>&& value)
        {
            bucket_.value().second = value_ptr_ = std::move(value);
            return *this;
        }

    private:
        explicit proxy(Bucket& bucket)
                : bucket_(bucket)
                , value_ptr_(bucket_.value().second.lock())
        { }

        Bucket& bucket_;
        std::shared_ptr<Value> value_ptr_;

        friend class weak_weak_unordered_map;
    };

    proxy operator[](const std::shared_ptr<const Key>& key)
    {
        Bucket* result_bucket;

        BaseClass::insert_helper_(
                *key,
                [&](Bucket& bucket) {
                    BaseClass::construct_bucket_(bucket, key,
                                                 std::shared_ptr<Value>{});
                    result_bucket = &bucket;
                },
                [&](Bucket& bucket) {
                    bucket.value().first = key;
                    bucket.value().second = std::shared_ptr<Value>{};
                    result_bucket = &bucket;
                },
                [&](Bucket& bucket) {
                    result_bucket = &bucket;
                });

        return proxy(*result_bucket);
    }
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void swap(weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
          weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

template <class Key, class Value, class Hash, class KeyEqual, class Allocator,
          class ValueEqual = std::equal_to<Value>>
bool submap(
        const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
        const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b,
        ValueEqual compare = std::equal_to<Value>())
{
    for (const auto& elem : a) {
        auto iter = b.find(*elem.first);
        if (iter == b.end() || !compare(*(*iter).second, *elem.second))
            return false;
    }

    return true;
}

template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool keys_subset(
        const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
        const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    submap(a, b, [](const auto&, const auto&) { return true; });
}

template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool operator==(const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
                const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    return submap(a, b) && keys_subset(b, a);
}

template <class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool operator!=(const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
                const weak_weak_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    return !(a == b);
}

} // end namespace weak
