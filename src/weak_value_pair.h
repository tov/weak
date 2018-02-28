#include "weak_traits.h"

#include <memory>

namespace weak {

/// A pair whose second component is a weak pointer.
///
/// The first element is owned directly.
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

    /// The first component.
    key_type first;
    /// The second component.
    value_weak_pointer second;

    /// Constructs or implicitly converts a weak pair from a strong pair.
    weak_value_pair(const strong_type& strong)
            : first(strong.first), second(strong.second)
    { }

    /// Constructs a weak pair from the given key and value.
    template <class K, class V>
    weak_value_pair(K&& key, V&& value)
            : first(std::forward<K>(key)), second(std::forward<V>(value))
    { }

    /// Is this weak pair expired?
    ///
    /// A weak value pair is expired if the second component is expired.
    bool expired() const
    {
        return second.expired();
    }

    /// Locks weak pair, producing a view that holds a strong pointer.
    view_type lock() const
    {
        return {first, second.lock()};
    }

    /// Gets a pointer to the key from a view pair.
    static const key_type* key(const view_type& view)
    {
        if (view.second)
            return &view.first;
        else
            return nullptr;
    }

    /// Gets a pointer to the key from a strong pair.
    static const key_type* key(const strong_type& strong)
    {
        return &strong.first;
    }

    /// Moves from a view pair, producing a strong pair.
    static strong_type move(view_type& view)
    {
        return {std::move(const_cast<key_type&>(view.first)),
                std::move(view.second)};
    }
};

} // end namespace weak
