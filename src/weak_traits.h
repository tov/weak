#pragma once

#include <memory>

namespace weak
{

/// Controls how a weak hash table uses its weak elements.
///
/// In particular, specializations of this struct provide a uniform interface
/// for dealing with both `std::weak_ptr` and the various weak pairs provided
/// by this library.
template <class T>
struct weak_traits
{
    /// Owns direct data, `shared_ptr` to indirect data.
    ///
    /// This type guarantees the presence of a key. It should be used when
    /// inserting into a weak hash table.
    using strong_type = typename T::strong_type;

    /// Borrows direct data, `shared_ptr` to indirect data.
    ///
    /// This type does not guarantee the presence of a key. It is used when
    /// viewing the contents of a weak hash table.
    using view_type = typename T::view_type;

    /// Const-borrows direct data, `shared_ptr` to indirect data.
    ///
    /// Like `view_type`, but for when viewing from a `const_iterator`.
    using const_view_type = typename T::const_view_type;

    /// The key type.
    ///
    /// For pairs, this is the `first` component. For plain `weak_ptr<T>`,
    /// this is `T`.
    using key_type = typename T::key_type;

    /// Projects a key pointer from a `view_type` or `strong_type`.
    ///
    /// If given a `view_type`, the result might be `nullptr`.
    template <class U>
    static const key_type* key(const U& view)
    {
        return T::key(view);
    }

    /// Steals a `view_type`, turning it into a `strong_type`.
    ///
    /// *UNCHECKED PRECONDITION*: the `view_type` is not expired.
    static strong_type move(view_type& view)
    {
        return T::move(view);
    }
};

/// Specialization for storing `std::weak_ptr<T>`.
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

    static const key_type* key(const view_type& view)
    {
        return view.get();
    }

    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

/// Specialization for storing `std::weak_ptr<const T>`.
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
