#include "weak_traits.h"

#include <memory>

namespace weak {

/// A pair both of whose components are weak pointers.
template<class Key, class Value,
        class KeyWeakPtr = std::weak_ptr<const Key>,
        class ValueWeakPtr = std::weak_ptr<Value>>
struct weak_weak_pair
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

    /// The first component.
    key_weak_pointer first;
    /// The second component.
    value_weak_pointer second;

    /// Constructs or implicitly converts a weak pair from a strong pair.
    weak_weak_pair(const strong_type& strong)
            : first(strong.first), second(strong.second)
    { }

    /// Constructs a weak pair from the given key and value.
    template <class K, class V>
    weak_weak_pair(K&& key, V&& value)
            : first(std::forward<K>(key)), second(std::forward<V>(value))
    { }

    /// Is this weak pair expired?
    ///
    /// A weak-weak pair is expired if either component is expired.
    bool expired() const
    {
        return first.expired() || second.expired();
    }

    /// Locks weak pair, producing a view that holds a strong pointer.
    view_type lock() const
    {
        if (auto key_ptr = first.lock())
            if (auto value_ptr = second.lock())
                return {key_ptr, value_ptr};

        return {nullptr, nullptr};
    }

    /// Gets a pointer to the key from a view pair.
    static const key_type* key(const view_type& view)
    {
        return view.first.get();
    }

    /// Moves from a view pair, producing a strong pair.
    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

} // end namespace weak
