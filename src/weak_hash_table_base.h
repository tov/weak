#pragma once

#include "detail/raw_vector.h"
#include "weak_traits.h"

#include <cassert>
#include <climits>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>

namespace weak {

/// \mainpage weak++: weak hash tables for C++17
///
/// ## Get It
///
/// The latest version is available [on GitHub](https://github.com/tov/weakpp).
///
/// ## Use It
/// This library provides four forms of weak hash tables:
///
///   - `weak_unordered_set`, which stores a set of `std::weak_ptr`s.
///
///   - `weak_key_unordered_map`, which maps `std::weak_ptr`s to values.
///
///   - `weak_value_unordered_map`, which maps keys to `std::weak_ptr`s.
///
///   - `weak_weak_unordered_map`, which maps `std::weak_ptr`s to
///     `std::weak_ptr`s.
///
/// Most of the interfaces of all four classes are common, and documented as
/// part of a shared base class `weak_hash_table_base`. All the constructors
/// may be found in that class as well.
///
/// Here is a tiny example:
///
/// ```cpp
/// weak_key_unordered_map<string, int> map;
/// CHECK( !map.member("hello") );
///
/// auto hello = make_shared<string>("hello");
/// map.insert({hello, 5});
/// CHECK( map.member("hello") );
///
/// hello = nullptr;
/// CHECK( !map.member("hello") );
/// ```
///
/// For a more realistic example, see
/// [`intern_table.h`](https://github.com/tov/weakpp/blob/master/test/intern_table.h)
/// and
/// [`intern_table.cpp`](https://github.com/tov/weakpp/blob/master/test/intern_table.cpp).
///

/// A weak Robin Hood hash table.
///
/// Provides the common functionality for all the weak hash tables, including
/// both sets and maps.
///
/// This is designed to work with element types that implement the
/// `weak_traits` interface. This includes `weak_ptr<T>` and the various weak
/// pair types provided by this library. Using a weak pair results in a weak
/// map, whereas using `weak_ptr<T>` results in a weak set. Usually this
/// should be instantiated through one of the derived classes, and this class
/// should not be used directly.
template <
        class T,
        class Hash = std::hash<typename weak_traits<T>::key_type>,
        class KeyEqual = std::equal_to<>,
        class Allocator = std::allocator<T>
>
class weak_hash_table_base
{
public:
    /// The actual value type stored by the hash table.
    using weak_value_type       = T;
    /// The instance of `weak_traits` for `weak_value_type`.
    using weak_trait            = weak_traits<weak_value_type>;
    /// The value type as viewed from an `iterator`.
    using view_value_type       = typename weak_trait::view_type;
    /// The value type as viewed from a `const_iterator`.
    using const_view_value_type = typename weak_trait::const_view_type;
    /// A fully owned and present value type, as required by insertion.
    using strong_value_type     = typename weak_trait::strong_type;
    /// The type of keys for this table.
    using key_type              = typename weak_trait::key_type;
    using hasher                = Hash;
    using key_equal             = KeyEqual;
    using allocator_type        = Allocator;

    /// The default number of buckets to allocate in a new hash table.
    static constexpr size_t default_bucket_count = 8;

    /// The default maximum load factor that determines when to grow.
    static constexpr float default_max_load_factor = 0.8;

private:
    // We're going to steal a bit from the hash codes to store a used bit..
    // So the number of hash bits is one less than the number of bits in size_t.
    static constexpr size_t number_of_hash_bits_ =
            sizeof(size_t) * CHAR_BIT - 2;

    static constexpr size_t hash_code_mask_ =
            (size_t(1) << number_of_hash_bits_) - 1;

protected:
    /// A bucket, which contains the stored `weak_value_type` along with
    /// some hidden metadata.
    class Bucket
    {
    public:
        /// Returns a reference to the stored `weak_value_type`.
        ///
        /// This should not be presumed initialized or uninitialized without further
        /// contextual information.
        weak_value_type& value()
        {
            return value_;
        }

        /// Returns a constant reference to the stored `weak_value_type`.
        ///
        /// This should not be presumed initialized or uninitialized without further
        /// contextual information.
        const weak_value_type& value() const
        {
            return value_;
        }

    private:
        weak_value_type value_;
        size_t          used_      : 1,
                        tombstone_ : 1,
                        hash_code_ : number_of_hash_bits_;
        // INVARIANT:
        //   - !(used_ && tombstone_)
        //   - if used_ then value_ is initialized, otherwise not
        //   - if used_ || tombstone_ then hash_code_ is initialized,
        //     otherwise not

        bool occupied_() const
        {
            return used_ && !value_.expired();
        }

        friend class weak_hash_table_base;
    };

private:
    using bucket_allocator_type =
        typename std::allocator_traits<allocator_type>
            ::template rebind_alloc<Bucket>;
    using weak_value_allocator_type =
        typename std::allocator_traits<allocator_type>
            ::template rebind_alloc<weak_value_type>;

    using vector_t = detail::raw_vector<Bucket, bucket_allocator_type>;

public:

    /// Constructs a new, empty weak hash table of default bucket count.
    weak_hash_table_base()
            : weak_hash_table_base(default_bucket_count)
    { }

    /// Constructs a new, empty weak hash table of the given
    /// bucket count.
    explicit weak_hash_table_base(
        size_t bucket_count,
        const hasher& hash = hasher(),
        const key_equal& equal = key_equal(),
        const allocator_type& allocator = allocator_type())
            : hasher_(hash)
            , equal_(equal)
            , bucket_allocator_(allocator)
            , weak_value_allocator_(allocator)
            , max_load_factor_(default_max_load_factor)
            , buckets_(bucket_count, bucket_allocator_)
            , size_(0)
    {
        init_buckets_();
    }

    /// Constructs a new, empty weak hash table of the given
    /// bucket count, using the given allocator.
    weak_hash_table_base(
        size_t bucket_count,
        const allocator_type& allocator)
            : weak_hash_table_base(bucket_count, hasher(), key_equal(), allocator)
    { }

    /// Constructs a new, empty weak hash table of the given bucket count,
    /// using the given hasher and allocator.
    weak_hash_table_base(
        size_t bucket_count,
        const hasher& hash,
        const allocator_type& allocator)
            : weak_hash_table_base(bucket_count, hash, key_equal(), allocator)
    { }

    /// Constructs a new, empty weak hash table of default bucket count,
    /// using the given allocator.
    explicit weak_hash_table_base(
        const allocator_type& allocator)
            : weak_hash_table_base(default_bucket_count,
                                   hasher(),
                                   key_equal(),
                                   allocator)
    { }

    /// Constructs a new weak hash table of the given bucket count,
    /// filling it with elements from the range [first, last).
    template <class InputIt>
    weak_hash_table_base(
        InputIt first, InputIt last,
        size_t bucket_count = default_bucket_count,
        const hasher& hash = hasher(),
        const key_equal& equal = key_equal(),
        const allocator_type& allocator = allocator_type())
            : weak_hash_table_base(bucket_count, hash, equal, allocator)
    {
        insert(first, last);
    }

    /// Constructs a new weak hash table of the given bucket count,
    /// using the given allocator, and filling it with elements from
    /// the range [first, last).
    template <class InputIt>
    weak_hash_table_base(
            InputIt first, InputIt last,
            size_t bucket_count,
            const allocator_type& allocator)
            : weak_hash_table_base(first, last, bucket_count,
                                   hasher(), key_equal(), allocator)
    { }

    /// Constructs a new weak hash table of the given bucket count,
    /// using the given allocator and hasher, and filling it with
    /// elements from the range [first, last).
    template <class InputIt>
    weak_hash_table_base(
        InputIt first, InputIt last,
        size_t bucket_count,
        const hasher& hash,
        const allocator_type& allocator)
            : weak_hash_table_base(first, last, bucket_count,
                                   hash, key_equal(), allocator)
    { }

    /// Copy constructor.
    weak_hash_table_base(const weak_hash_table_base& other)
            : weak_hash_table_base(other, other.get_allocator())
    { }

    /// Copy constructor with allocator.
    weak_hash_table_base(const weak_hash_table_base& other,
                         const allocator_type& allocator)
            : weak_hash_table_base(other.min_bucket_count_(),
                                   other.hasher_,
                                   other.equal_,
                                   allocator)
    {
        max_load_factor(other.max_load_factor());
        insert(other.begin(), other.end());
    }

    /// Move constructor.
    weak_hash_table_base(weak_hash_table_base&& other)
            : weak_hash_table_base(0)
    {
        swap(other);
    }

    /// Move constructor with allocator.
    weak_hash_table_base(weak_hash_table_base&& other,
                         const allocator_type& allocator)
            : weak_hash_table_base(0)
    {
        swap(other);
        bucket_allocator_ = allocator;
        weak_value_allocator_ = allocator;
    }

    /// Constructs from an initializer list of values.
    weak_hash_table_base(std::initializer_list<strong_value_type> elements,
                         size_t bucket_count = default_bucket_count,
                         const hasher& hash = hasher(),
                         const key_equal& equal = key_equal(),
                         const allocator_type& allocator = allocator_type())
            : weak_hash_table_base(bucket_count, hash, equal, allocator)
    {
        insert(elements.begin(), elements.end());
    }

    /// Constructs from an initializer list of values, with the given
    /// bucket count and allocator.
    weak_hash_table_base(std::initializer_list<strong_value_type> elements,
                         size_t bucket_count,
                         const allocator_type& allocator)
            : weak_hash_table_base(elements, bucket_count, hasher(),
                                   key_equal(), allocator)
    { }

    /// Construct from an initializer list of values, with the given
    /// bucket count, hasher, and allocator.
    weak_hash_table_base(std::initializer_list<strong_value_type> elements,
                         size_t bucket_count,
                         const hasher& hash,
                         const allocator_type& allocator)
            : weak_hash_table_base(elements, bucket_count, hash,
                                   key_equal(), allocator)
    { }

    /// Destructor.
    ~weak_hash_table_base()
    {
        clear();
    }

    /// Copy-assignment.
    weak_hash_table_base& operator=(const weak_hash_table_base& other)
    {
        clear();
        insert(other.begin(), other.end());
        return *this;
    }

    /// Move-assignment.
    weak_hash_table_base& operator=(weak_hash_table_base&& other)
    {
        clear();
        swap(other);
        return *this;
    }

    /// Returns the allocator.
    allocator_type get_allocator() const
    {
        return {bucket_allocator_};
    }

    /// Returns the key equality predicate.
    key_equal key_eq() const
    {
        return equal_;
    }

    /// Returns the hash function.
    hasher hash_function() const
    {
        return hasher_;
    }

    /// If weak pointers have expired, an empty hash table may appear
    /// non-empty.
    bool empty() const
    {
        return size_ == 0;
    }

    /// The number of open-addressed buckets.
    size_t bucket_count() const
    {
        return buckets_.size();
    }

    /// The current load factor.
    ///
    /// This over-approximates the proportion of used buckets.
    float load_factor() const
    {
        if (bucket_count() == 0) return 1;
        return float(size_) / bucket_count();
    }

    /// The maximum load factor, exceeding which will trigger growth.
    float max_load_factor() const
    {
        return max_load_factor_;
    }

    /// Sets the maximum load factor.
    //
    /// *PRECONDITION*: 0 < `new_value` < 1
    void max_load_factor(float new_value)
    {
        assert(0 < new_value && new_value < 1);
        max_load_factor_ = new_value;
    }

    /// Note that because pointers may expire without the table finding
    /// out, size() is generally an overapproximation of the number of
    /// elements in the hash table.
    size_t size() const
    {
        return size_;
    }

    /// Removes all elements.
    void clear()
    {
        for (auto& bucket : buckets_) {
            if (bucket.used_) {
                destroy_bucket_(bucket, 0);
            }
        }

        size_ = 0;
    }

    /// Cleans up expired elements. After this, `size()` is accurate.
    void remove_expired()
    {
        for (auto& bucket : buckets_) {
            if (bucket.used_ && bucket.value_.expired()) {
                destroy_bucket_(bucket, 1);
                --size_;
            }
        }
    }

    /// Reserves room for `extra` additional elements, sort of.
    void reserve(size_t extra)
    {
        remove_expired();
        resize_(std::max(size() + extra, min_bucket_count_()));
    }

    /// Inserts an element.
    void insert(const strong_value_type& value)
    {
        size_t hash_code = hash_(*weak_trait::key(value));
        insert_(hash_code, value);
    }

    /// Inserts an element.
    void insert(strong_value_type&& value)
    {
        size_t hash_code = hash_(*weak_trait::key(value));
        insert_(hash_code, std::move(value));
    }

    /// Inserts a range of elements.
    template <typename InputIter>
    void insert(InputIter start, InputIter limit)
    {
        for ( ; start != limit; ++start)
            insert(*start);
    }

    /// Erases the element if the given key, returning whether an
    /// element was actually erased.
    bool erase(const key_type& key)
    {
        if (auto bucket_index = lookup_(key)) {
            destroy_bucket_(buckets_[*bucket_index], 1);
            --size_;
            return true;
        } else {
            return false;
        }
    }

    /// Swaps this weak hash table with another in constant time.
    void swap(weak_hash_table_base& other)
    {
        using std::swap;
        swap(buckets_, other.buckets_);
        swap(size_, other.size_);
        swap(hasher_, other.hasher_);
        swap(equal_, other.equal_);
        swap(bucket_allocator_, other.bucket_allocator_);
        swap(weak_value_allocator_, other.weak_value_allocator_);
        swap(max_load_factor_, other.max_load_factor_);
    }

    /// Is the given key mapped by this hash table?
    template <class KeyLike>
    bool member(const KeyLike& key) const
    {
        return lookup_(key) != std::nullopt;
    }

    /// Counts the number of times the `key` appears (0 or 1).
    template <class KeyLike>
    size_t count(const KeyLike& key) const
    {
        return member(key)? 1 : 0;
    }

    class iterator;
    class const_iterator;

    /// Returns an iterator to the given key, or `this->end()` if not found.
    template <class KeyLike>
    iterator find(const KeyLike& key)
    {
        return make_iterator_(lookup_(key));
    }

    /// Returns an iterator to the given key, or `this->end()` if not found.
    template <class KeyLike>
    const_iterator find(const KeyLike& key) const
    {
        return make_iterator_(lookup_(key));
    }

    /// Returns an iterator to the beginning of the hash table.
    iterator begin()
    {
        return make_iterator_({0});
    }

    /// Returns an iterator past the end of the hash table.
    iterator end()
    {
        return make_iterator_(std::nullopt);
    }

    /// Returns a constant iterator to the beginning of the hash table.
    const_iterator begin() const
    {
        return cbegin();
    }

    /// Returns a constant iterator past the end of the hash table.
    const_iterator end() const
    {
        return cend();
    }

    /// Returns a constant iterator to the beginning of the hash table.
    const_iterator cbegin() const
    {
        return make_iterator_({0});
    }

    /// Returns a constant iterator past the end of the hash table.
    const_iterator cend() const
    {
        return make_iterator_(std::nullopt);
    }

private:
    iterator make_iterator_(std::optional<size_t> bucket_index)
    {
        return {bucket_index? &buckets_[*bucket_index] : buckets_.end(),
                buckets_.end()};
    }

    const_iterator make_iterator_(std::optional<size_t> bucket_index) const
    {
        return {bucket_index? &buckets_[*bucket_index] : buckets_.end(),
                buckets_.end()};
    }

private:
    hasher hasher_;
    key_equal equal_;
    bucket_allocator_type bucket_allocator_;
    weak_value_allocator_type weak_value_allocator_;
    float max_load_factor_;

    vector_t buckets_;
    size_t size_;

    bool needs_to_grow_()
    {
        return load_factor() > max_load_factor() || size() >= bucket_count();
    }

    void maybe_grow_()
    {
        if (needs_to_grow_()) {
            remove_expired();
            if (needs_to_grow_())
                resize_(std::max(2 * bucket_count(), size() + 1));
        }
    }

    size_t min_bucket_count_() const noexcept
    {
        return size_t(size() / max_load_factor()) + 1;
    }

    void resize_(size_t new_bucket_count)
    {
        assert(new_bucket_count > size_);

        using std::swap;
        vector_t old_buckets(new_bucket_count, bucket_allocator_);
        swap(old_buckets, buckets_);
        size_ = 0;
        init_buckets_();

        for (Bucket& bucket : old_buckets) {
            if (bucket.used_) {
                view_value_type value = bucket.value_.lock();
                if (weak_trait::key(value)) {
                    insert_(bucket.hash_code_, weak_trait::move(value));
                }
            }
        }
    }

    template <class KeyLike>
    std::optional<size_t> lookup_(const KeyLike& key) const
    {
        size_t hash_code = hash_(key);
        size_t pos = which_bucket_(hash_code);
        size_t dist = 0;

        for (;;) {
            const Bucket& bucket = buckets_[pos];

            if (bucket.tombstone_ || bucket.used_) {
                if (dist >
                    probe_distance_(pos, which_bucket_(bucket.hash_code_)))
                    return std::nullopt;
            }

            if (!bucket.used_)
                return std::nullopt;

            if (!bucket.tombstone_ && hash_code == bucket.hash_code_) {
                auto bucket_value_locked = bucket.value_.lock();
                if (const auto* bucket_key =
                        weak_trait::key(bucket_value_locked))
                    if (equal_(key, *bucket_key))
                        return {pos};
            }

            pos = next_bucket_(pos);
            ++dist;
        }
    }

    /// Places `value` in the table, starting at `pos` and moving forward.
    void steal_(size_t hash_code, size_t pos, strong_value_type&& value)
    {
        size_t dist = probe_distance_(pos, which_bucket_(hash_code));

        for (;;) {
            Bucket& bucket = buckets_[pos];

            if (!bucket.used_ || bucket.tombstone_) {
                construct_bucket_(bucket, std::move(value));
                bucket.hash_code_ = hash_code;
                return;
            }

            if (bucket.value_.expired()) {
                bucket.value_ = std::move(value);
                bucket.hash_code_ = hash_code;
                return;
            }

            size_t existing_distance =
                    probe_distance_(pos, which_bucket_(bucket.hash_code_));
            if (dist > existing_distance) {
                auto bucket_locked = bucket.value_.lock();
                bucket.value_ = std::exchange(value, weak_trait::move(bucket_locked));
                // swap doesn't work because bitfield:
                bucket.hash_code_ = std::exchange(hash_code, size_t(bucket.hash_code_));
                dist = existing_distance;
            }

            pos = next_bucket_(pos);
            ++dist;
        }
    }

    void insert_(size_t hash_code, const strong_value_type& value)
    {
        insert_helper_(hash_code, *weak_trait::key(value),
                       [&](Bucket& bucket) {
                           construct_bucket_(bucket, value);
                       },
                       [&](Bucket& bucket) {
                           bucket.value_ = value;
                       },
                       [&](Bucket& bucket) {
                           bucket.value_ = value;
                       });
    }

    void insert_(size_t hash_code, strong_value_type&& value)
    {
        insert_helper_(hash_code, *weak_trait::key(value),
                       [&](Bucket& bucket) {
                           construct_bucket_(bucket, std::move(value));
                       },
                       [&](Bucket& bucket) {
                           bucket.value_ = std::move(value);
                       },
                       [&](Bucket& bucket) {
                           bucket.value_ = std::move(value);
                       });
    }

protected:
    /// Given a bucket, which must be uninitialized emplaces the value in
    /// it and marks it used.
    template <class... Args>
    void construct_bucket_(Bucket& bucket, Args&&... args)
    {
        std::allocator_traits<weak_value_allocator_type>::construct(
                weak_value_allocator_,
                &bucket.value_,
                std::forward<Args>(args)...);
        bucket.used_ = 1;
        bucket.tombstone_ = 0;
    }

    /// Expects to call exactly one of the parameters `on_uninit`, `on_init`,
    /// and `on_found`, which will (re-)initialize the bucket with the given key.
    template <class OnUninit, class OnInit, class OnFound>
    void insert_helper_(const key_type& key,
                        OnUninit on_uninit, OnInit on_init, OnFound on_found)
    {
        insert_helper_(hash_(key), key, on_uninit, on_init, on_found);
    }

private:

    /// Expects to call exactly one of the parameters `on_uninit`, `on_init`,
    /// and `on_found`, which will (re-)initialize the bucket with the given key.
    ///
    /// PRECONDITION: hash_code == hash_(key)
    template <class OnUninit, class OnInit, class OnFound>
    void insert_helper_(size_t hash_code, const key_type& key,
                        OnUninit on_uninit, OnInit on_init, OnFound on_found)
    {
        maybe_grow_();

        size_t pos = which_bucket_(hash_code);
        size_t dist = 0;

        for (;;) {
            Bucket& bucket = buckets_[pos];

            if (!bucket.used_ || bucket.tombstone_) {
                on_uninit(bucket);
                bucket.hash_code_ = hash_code;
                ++size_;
                return;
            }

            auto bucket_locked = bucket.value_.lock();
            auto bucket_key = weak_trait::key(bucket_locked);
            if (!bucket_key) {
                on_init(bucket);
                bucket.hash_code_ = hash_code;
                return;
            }

            // If not expired, but matches the value to insert, replace.
            if (hash_code == bucket.hash_code_ && equal_(key, *bucket_key)) {
                on_found(bucket);
                return;
            }

            // Otherwise, we check the probe distance.
            size_t existing_distance =
                    probe_distance_(pos, which_bucket_(bucket.hash_code_));
            if (dist > existing_distance) {
                steal_(bucket.hash_code_, next_bucket_(pos),
                       weak_trait::move(bucket_locked));
                on_init(bucket);
                bucket.hash_code_ = hash_code;
                return;
            }

            pos = next_bucket_(pos);
            ++dist;
        }
    }

    template <class KeyLike>
    size_t hash_(const KeyLike& key) const
    {
        return hasher_(key) & hash_code_mask_;
    }

    void destroy_bucket_(Bucket& bucket, size_t tombstone = 0)
    {
        std::allocator_traits<weak_value_allocator_type>::destroy(
                weak_value_allocator_,
                &bucket.value_);
        bucket.used_ = 0;
        bucket.tombstone_ = tombstone;
    }

    void init_buckets_()
    {
        for (auto& bucket : buckets_) {
            bucket.used_ = 0;
            bucket.tombstone_ = 0;
        }
    }

    size_t next_bucket_(size_t pos) const
    {
        return (pos + 1) % bucket_count();
    }

    size_t probe_distance_(size_t actual, size_t preferred) const
    {
        if (actual >= preferred)
            return actual - preferred;
        else
            return actual + bucket_count() - preferred;
    }

    size_t which_bucket_(size_t hash_code) const
    {
        return hash_code % bucket_count();
    }
};

/// An iterator over the values of the hash table.
///
/// This iterator is invalidated by any operation that changes the hash
/// table, including a shared pointer expiring.
///
/// This iterator may allow modifying the values, but it does not allow
/// modifying the keys, since that would destroy the hash invariant.
template <
        class T,
        class Hash,
        class KeyEqual,
        class Allocator
>
class weak_hash_table_base<T, Hash, KeyEqual, Allocator>::iterator
        : public std::iterator<std::forward_iterator_tag, T>
{
private:
    friend class weak_hash_table_base;
    friend class const_iterator;

    using base_t = typename vector_t::iterator;

    iterator(base_t start, base_t limit)
            : base_(start), limit_(limit)
    {
        find_next_();
    }

public:
    /// Provides a pointer view of the iterator.
    ///
    /// Note that because the value type of the iterator does not exist until
    /// derereferenced, this operator allocates to return a pointer to it. Thus,
    /// `operator*() const` is probably more efficient.
    std::unique_ptr<view_value_type> operator->() const
    {
        return std::make_unique<view_value_type>(operator*());
    }

    /// Returns the value indicated by the iterator.
    view_value_type operator*() const
    {
        return base_->value_.lock();
    }

    /// Advances the iterator.
    iterator& operator++()
    {
        ++base_;
        find_next_();
        return *this;
    }

    /// Advances the iterator.
    iterator operator++(int)
    {
        auto old = *this;
        ++*this;
        return old;
    }

    /// Iterator equality.
    bool operator==(iterator other) const
    {
        return base_ == other.base_;
    }

    /// Iterator disequality.
    bool operator!=(iterator other) const
    {
        return base_ != other.base_;
    }

private:
    // Invariant: if base_ != limit_ then base_->occupied();
    base_t base_;
    base_t limit_;

    void find_next_()
    {
        while (base_ != limit_ && !base_->occupied_())
            ++base_;
    }
};

/// A constant iterator over the values of the hash table.
///
/// This iterator is invalidated by any operation that changes the hash
/// table, including a shared pointer expiring.
template <
        class T,
        class Hash,
        class KeyEqual,
        class Allocator
>
class weak_hash_table_base<T, Hash, KeyEqual, Allocator>::const_iterator
        : public std::iterator<std::forward_iterator_tag, const T>
{
private:
    friend class weak_hash_table_base;

    using base_t = typename vector_t::const_iterator;

    const_iterator(base_t start, base_t limit)
            : base_(start), limit_(limit)
    {
        find_next_();
    }

public:
    /// Implicit conversion from `iterator` to `const_iterator`.
    const_iterator(iterator other)
            : base_(other.base_), limit_(other.limit_)
    { }

    /// Provides a pointer view of the iterator.
    ///
    /// Note that because the value type of the iterator does not exist until
    /// derereferenced, this operator allocates to return a pointer to it. Thus,
    /// `operator*() const` is probably more efficient.
    std::unique_ptr<const_view_value_type> operator->() const
    {
        return std::make_unique<const_view_value_type>(operator*());
    }

    /// Returns the value indicated by the iterator.
    const_view_value_type operator*() const
    {
        return base_->value_.lock();
    }

    /// Advances the iterator.
    const_iterator& operator++()
    {
        ++base_;
        find_next_();
        return *this;
    }

    /// Advances the iterator.
    const_iterator operator++(int)
    {
        auto old = *this;
        ++*this;
        return old;
    }

    /// Iterator equality.
    bool operator==(const_iterator other) const
    {
        return base_ == other.base_;
    }

    /// Iterator disequality.
    bool operator!=(const_iterator other) const
    {
        return base_ != other.base_;
    }

private:
    // Invariant: if base_ != limit_ then base_->occupied();
    base_t base_;
    base_t limit_;

    void find_next_()
    {
        while (base_ != limit_ && !base_->occupied_())
            ++base_;
    }
};

/// Swaps the contents of two weak hash tables in constant time.
template <class T, class Hash, class KeyEqual, class Allocator>
void swap(weak_hash_table_base<T, Hash, KeyEqual, Allocator>& a,
          weak_hash_table_base<T, Hash, KeyEqual, Allocator>& b)
{
    a.swap(b);
}

} // end namespace weak

