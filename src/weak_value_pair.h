#include "weak_traits.h"

#include <memory>

namespace weak {

template <class Key, class Value,
        class ValueWeakPtr = std::weak_ptr<Value>>
struct weak_value_pair
{
    using key_type = Key;
    using value_type = Value;
    using value_pointer = typename weak_traits<ValueWeakPtr>::strong_type;
    using value_weak_pointer = ValueWeakPtr;
    using strong_type = std::pair<key_type, value_pointer>;
    using view_type = std::pair<const key_type&, value_pointer>;
    using const_view_type = view_type;

    key_type first;
    value_weak_pointer second;

    weak_value_pair(const strong_type& strong)
            : first(strong.first), second(strong.second)
    { }

    bool expired() const
    {
        return second.expired();
    }

    view_type lock() const
    {
        return {first, second.lock()};
    }

    static const key_type* key(const view_type& view)
    {
        if (view.second)
            return &view.first;
        else
            return nullptr;
    }

    static const key_type* key(const strong_type& strong)
    {
        return &strong.first;
    }

    static strong_type move(view_type& view)
    {
        return {std::move(const_cast<key_type&>(view.first)),
                std::move(view.second)};
    }
};

} // end namespace weak
