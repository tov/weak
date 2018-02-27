cmake_minimum_required(VERSION 3.3)
project(weak CXX)

include_directories(src)
include_directories(3rd_party)

macro (add_executable17 name)
    add_executable(${name} ${ARGN})
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED On)
endmacro (add_executable17)

add_executable17(pairs_test
        test/catch_main.cpp
        test/pairs_test.cpp
        src/weak_traits.h
        src/weak_key_pair.h
        src/weak_value_pair.h
        src/weak_pair.h)

add_executable17(raw_vector_test
        test/catch_main.cpp
        test/raw_vector_test.cpp
        src/detail/raw_vector.h)

add_executable17(weak_hash_table_test
        test/catch_main.cpp
        test/weak_unordered_set_test.cpp
        test/weak_unordered_map_test.cpp
        src/weak_unordered_set.h
        src/weak_weak_unordered_map.h
        src/weak_key_unordered_map.h
        src/weak_value_unordered_map.h
        src/detail/weak_unordered_map_base.h
        src/detail/weak_hash_table_base.h)


