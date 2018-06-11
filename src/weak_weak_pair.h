#include "weak_traits.h"

#include <memory>

namespace weak {

/// A pair both of whose components are weak pointers.
template<class T1, class T2,
         class WeakPtr1 = std::weak_ptr<const T1>,
         class WeakPtr2 = std::weak_ptr<T2>>
struct weak_weak_pair
{
    using first_type = T1;
    using second_type = T2;
    using first_pointer = typename weak_traits<WeakPtr1>::strong_type;
    using second_pointer = typename weak_traits<WeakPtr2>::strong_type;
    using second_const_pointer = typename weak_traits<WeakPtr2>::const_view_type;
    using first_weak_pointer = WeakPtr1;
    using second_weak_pointer = WeakPtr2;
    using key_type = const first_type;
    using strong_type = std::pair<first_pointer, second_pointer>;
    using view_type = strong_type;
    using const_view_type = std::pair<first_pointer, second_const_pointer>;

    /// The first component.
    first_weak_pointer first;
    /// The second component.
    second_weak_pointer second;

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

    static const_view_type view(const strong_type& strong)
    {
        return {strong.first, strong.second};
    }

    /// Gets a pointer to the key from a view pair.
    static const first_type* key(const_view_type& view)
    {
        return view.first.get();
    }

    /// Gets a pointer to the key from a strong pair.
    static const first_type& strong_key(const strong_type& strong)
    {
        return *strong.first;
    }

    /// Moves from a view pair, producing a strong pair.
    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

} // end namespace weak
