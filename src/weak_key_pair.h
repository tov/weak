#include "weak_traits.h"

#include <memory>

namespace weak {

template <class Key, class Value,
        class KeyWeakPtr = std::weak_ptr<const Key>>
struct weak_key_pair
{
    using key_type = Key;
    using value_type = Value;
    using key_pointer = typename weak_traits<KeyWeakPtr>::strong_type;
    using key_weak_pointer = KeyWeakPtr;
    using strong_type = std::pair<key_pointer, value_type>;
    using view_type = std::pair<key_pointer, value_type&>;
    using const_view_type = std::pair<key_pointer, const value_type&>;

    key_weak_pointer first;
    value_type second;

    weak_key_pair(const strong_type& strong)
            : first(strong.first), second(strong.second)
    { }

    bool expired() const
    {
        return first.expired();
    }

    view_type lock()
    {
        return {first.lock(), second};
    }

    const_view_type lock() const
    {
        return {first.lock(), second};
    }

    static const key_type* key(const view_type& view)
    {
        if (view.first)
            return view.first.get();
        else
            return nullptr;
    }

    static const key_type& key(const strong_type& strong)
    {
        return strong.first.get();
    }

    static strong_type move(view_type& view)
    {
        return {std::move(view.first), std::move(view.second)};
    }
};

} // end namespace weak
