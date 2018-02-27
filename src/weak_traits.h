#pragma once

#include <memory>

namespace weak
{

template <class T>
struct weak_traits
{
    /// strong_type guarantees the presence of a key.
    using strong_type = typename T::strong_type;

    /// view_type does not.
    using view_type = typename T::view_type;

    /// as viewed from a const_iterator.
    using const_view_type = typename T::const_view_type;

    /// the type of keys
    using key_type = typename T::key_type;

    template <class U>
    static const key_type* key(const U& view)
    {
        return T::key(view);
    }

    /// steals a view_type, turning it into a strong_type
    /// PRECONDITION: the view_type is not expired
    static const strong_type move(view_type& view)
    {
        return T::move(view);
    }
};

template <class T>
struct weak_traits<std::weak_ptr<T>>
{
    using strong_type = std::shared_ptr<T>;
    using view_type = strong_type;
    using const_view_type = view_type;
    using key_type = T;

    static const view_type& view(const strong_type& strong)
    {
        return strong;
    }

    // This works for both view_type and strong_type, since they are the
    // same.
    static const key_type* key(const view_type& view)
    {
        return view.get();
    }

    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

template <class T>
struct weak_traits<std::weak_ptr<const T>>
{
    using strong_type = std::shared_ptr<const T>;
    using view_type = strong_type;
    using const_view_type = view_type;
    using key_type = const T;

    static const view_type& view(const strong_type& strong)
    {
        return strong;
    }

    // This works for both view_type and strong_type, since they are the
    // same.
    static key_type* key(const view_type& view)
    {
        return view? view.get() : nullptr;
    }

    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

} // end namespace weak
