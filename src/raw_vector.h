#pragma once

#include <stdexcept>
#include <iterator>
#include <memory>

namespace weak::detail {

/// A `raw_vector` is a fixed-sized, non-copyable vector that owns but does
/// not initialize its memory.
template<class T, class Allocator = std::allocator<T>>
class raw_vector
{
public:
    using value_type = T;
    using allocator_type = Allocator;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using reference = value_type&;
    using const_reference = value_type const&;

    raw_vector()
            : raw_vector(0)
    {}

    explicit raw_vector(size_t size,
                        const allocator_type& allocator = allocator_type())
            : allocator_(allocator),
              size_(size),
              data_(allocate_(allocator_, size_))
    {}

    raw_vector(const raw_vector&) = delete;

    raw_vector(raw_vector&& other) noexcept : raw_vector()
    {
        swap(other);
    }

    raw_vector& operator=(raw_vector&& other) = delete;
    raw_vector& operator=(const raw_vector&) = delete;

    ~raw_vector()
    {
        deallocate_();
    }

    bool empty() const
    {
        return size_ == 0;
    }

    size_t size() const
    {
        return size_;
    }

    void swap(raw_vector& other)
    {
        using std::swap;
        swap(allocator_, other.allocator_);
        swap(size_, other.size_);
        swap(data_, other.data_);
    }

    const_reference operator[](size_t index) const
    {
        return data_[index];
    }

    reference operator[](size_t index)
    {
        return data_[index];
    }

    const_reference at(size_t index) const
    {
        if (index >= size_) throw std::range_error("raw_vector");
        return data_[index];
    }

    reference at(size_t index)
    {
        if (index >= size_) throw std::range_error("raw_vector");
        return data_[index];
    }

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { return data_; }

    const_iterator begin() const { return data_; }

    const_iterator cbegin() const { return data_; }

    iterator end() { return data_ + size_; }

    const_iterator end() const { return data_ + size_; }

    const_iterator cend() const { return data_ + size_; }

    reverse_iterator rbegin() { return {end()}; }

    const_reverse_iterator rbegin() const { return {end()}; }

    const_reverse_iterator crbegin() const { return {end()}; }

    reverse_iterator rend() { return {begin()}; }

    const_reverse_iterator rend() const { return {begin()}; }

    const_reverse_iterator crend() const { return {begin()}; }

private:
    allocator_type allocator_;
    size_t size_;
    T* data_;

    using allocator_trait = std::allocator_traits<allocator_type>;

    static T* allocate_(allocator_type& allocator, size_t size)
    {
        return allocator_trait::allocate(allocator, size);
    }

    void deallocate_()
    {
        allocator_trait::deallocate(allocator_, data_, size_);
    }
};

template<class T, class Allocator>
void swap(raw_vector<T, Allocator>& a, raw_vector<T, Allocator>& b)
{
    a.swap(b);
}

} // end namespace intersections::util
