#include "weak_traits.h"

#include <memory>

namespace weak {

/// A pair whose second component is a weak pointer.
///
/// The first element is owned directly.
template <class T1, class T2,
          class WeakPtr2 = std::weak_ptr<T2>>
struct weak_value_pair
{
    using first_type = T1;
    using second_type = T2;
    using second_pointer = typename weak_traits<WeakPtr2>::strong_type;
    using second_const_pointer = typename weak_traits<WeakPtr2>::const_view_type;
    using second_weak_pointer = WeakPtr2;
    using key_type = const first_type;
    using strong_type = std::pair<first_type, second_pointer>;
    using view_type = std::pair<const first_type&, second_pointer>;
    using const_view_type = std::pair<const first_type&, second_const_pointer>;

    /// The first component.
    first_type first;
    /// The second component.
    second_weak_pointer second;

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
    static const first_type* key(const view_type& view)
    {
        if (view.second)
            return &view.first;
        else
            return nullptr;
    }

    /// Gets a pointer to the key from a strong pair.
    static const first_type* key(const strong_type& strong)
    {
        return &strong.first;
    }

    /// Moves from a view pair, producing a strong pair.
    static strong_type move(view_type& view)
    {
        return {std::move(const_cast<first_type&>(view.first)),
                std::move(view.second)};
    }
};

} // end namespace weak
