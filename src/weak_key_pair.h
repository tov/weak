#include "weak_traits.h"

#include <memory>

namespace weak {

/// A pair whose first component is a weak pointer.
///
/// The second element is owned directly.
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

    /// The first component.
    key_weak_pointer first;
    /// The second component.
    value_type second;

    /// Constructs or implicitly converts a weak pair from a strong pair.
    weak_key_pair(const strong_type& strong)
            : first(strong.first), second(strong.second)
    { }

    /// Constructs a weak pair from the given key and value.
    template <class K, class V>
    weak_key_pair(K&& key, V&& value)
            : first(std::forward<K>(key)), second(std::forward<V>(value))
    { }

    /// Is this weak pair expired?
    ///
    /// A weak key pair is expired if the first component is expired.
    bool expired() const
    {
        return first.expired();
    }

    /// Locks weak pair, producing a view that holds a strong pointer.
    view_type lock()
    {
        return {first.lock(), second};
    }

    /// Locks weak pair, producing a view that holds a strong pointer.
    const_view_type lock() const
    {
        return {first.lock(), second};
    }

    /// Gets a pointer to the key from a view pair.
    static const key_type* key(const view_type& view)
    {
        return view.first.get();
    }

    /// Gets a pointer to the key from a strong pair.
    static const key_type* key(const strong_type& strong)
    {
        return strong.first.get();
    }

    /// Moves from a view pair, producing a strong pair.
    static strong_type move(view_type& view)
    {
        return {std::move(view.first), std::move(view.second)};
    }
};

} // end namespace weak
