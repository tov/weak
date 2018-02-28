# weak: weak hash tables for C++

This library provides four forms of weak hash tables:

  - `weak_unordered_set`, which stores a set of `std::weak_ptr`s.

  - `weak_key_unordered_map`, which maps `std::weak_ptr`s to values.

  - `weak_value_unordered_map`, which maps keys to `std::weak_ptr`s.

  - `weak_weak_unordered_map`, which maps `std::weak_ptr`s to
    `std::weak_ptr`s.

Documentation is [here](https://tov.github.io/weak/).

This library is header-only, but tests can be built with CMake.
