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

    /// Views the strong type back at the const view type.
    static const_view_type view(const strong_type& strong)
    {
        return T::view(strong);
    }

    /// Projects a key pointer from a `view_type`.
    ///
    /// The result might be `nullptr`.
    static const key_type* key(const_view_type& view)
    {
        return T::key(view);
    }

    /// Projects a key pointer from a `strong_type`.
    static const key_type& strong_key(const strong_type& strong) {
        return T::strong_key(strong);
    }

    /// Steals a `view_type`, turning it into a `strong_type`.
    ///
    /// *UNCHECKED PRECONDITION*: the `view_type` is not expired.
    static strong_type move(view_type& view)
    {
        return T::move(view);
    }
};

///
template <class T>
struct weak_traits<std::weak_ptr<T>>
{
    using strong_type = std::shared_ptr<T>;
    using view_type = strong_type;
    using const_view_type = view_type;
    using key_type = T;

    static const_view_type view(strong_type& strong)
    {
        return strong;
    }

    static const key_type* key(const_view_type& view)
    {
        return view.get();
    }

    static const key_type& strong_key(const strong_type& strong)
    {
        return *strong;
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

    static const_view_type view(const strong_type& strong)
    {
        return strong;


    static key_type* key(const_view_type& view)
    {
        return view? view.get() : nullptr;
    }

    static key_type& strong_key(const strong_type& strong)
    {
        return *strong;
    }

    static strong_type move(view_type& view)
    {
        return std::move(view);
    }
};

template <class WeakPtr>
struct by_ptr
{
    by_ptr(WeakPtr p) : ptr(p) { }
    by_ptr(typename weak_traits<WeakPtr>::strong_type p) : ptr(p) { }
    WeakPtr ptr;

    auto lock() const
    {
        return ptr.lock();
    }
};

template <class WeakPtr>
struct weak_traits<by_ptr<WeakPtr>>
{
    using weak_ptr_traits = weak_traits<WeakPtr>;
    using strong_type = typename weak_ptr_traits::strong_type;
    using view_type = typename weak_ptr_traits::view_type;
    using const_view_type = typename weak_ptr_traits::const_view_type;
    using key_type = const_view_type;

    static const_view_type view(const strong_type& strong)
    {
        return weak_ptr_traits::view(strong);
    }

    static const key_type* key(const_view_type& view) {
        return &view;
    }

    static key_type strong_key(const strong_type& strong) {
        return weak_ptr_traits::view(strong);
    }

    static strong_type move(view_type& view)
    {
        return weak_ptr_traits::move(view);
    }
};

} // end namespace weak
