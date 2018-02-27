#pragma once

#include "detail/weak_hash_table_base.h"
#include "detail/weak_unordered_map_base.h"
#include "weak_traits.h"
#include "weak_pair.h"

namespace weak {

template<
        class Key, class Value,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<weak_pair<Key, Value>>>
class weak_key_weak_value_unordered_map
        : public detail::weak_unordered_map_base<weak_pair<Key, Value>,
                                                 Hash, KeyEqual, Allocator>
{
    using BaseClass = detail::weak_unordered_map_base<weak_pair<Key, Value>,
                                                      Hash, KeyEqual,
                                                      Allocator>;
    using Bucket = typename BaseClass::Bucket;
public:
    using BaseClass::weak_unordered_map_base;

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

        friend class weak_key_weak_value_unordered_map;
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
void swap(weak_key_weak_value_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a,
          weak_key_weak_value_unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

} // end namespace weak
