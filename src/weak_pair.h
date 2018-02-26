#include "weak_traits.h"

#include <memory>

namespace weak {

template<class Key, class Value,
        class KeyWeakPtr = std::weak_ptr<const Key>,
        class ValueWeakPtr = std::weak_ptr<Value>>
struct weak_pair
{
    using key_type = Key;
    using value_type = Value;
    using key_pointer = typename weak_traits<KeyWeakPtr>::strong_type;
    using value_pointer = typename weak_traits<ValueWeakPtr>::strong_type;
    using key_weak_pointer = KeyWeakPtr;
    using value_weak_pointer = ValueWeakPtr;
    using strong_type = std::pair<key_pointer, value_pointer>;
    using view_type = strong_type;
    using const_view_type = view_type;

    key_weak_pointer first;
    value_weak_pointer second;

    weak_pair(const strong_type& strong)
            : first(strong.first), second(strong.second) {}

    bool expired() const
    {
        return first.expired() || second.expired();
    }

    view_type lock() const
    {
        if (auto key_ptr = first.lock())
            if (auto value_ptr = second.lock())
                return {key_ptr, value_ptr};

        return {nullptr, nullptr};
    }

    static const key_type* key(const view_type& view)
    {
        if (view.first)
            return view.first.get();
        else
            return nullptr;
    }

    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

} // end namespace weak
