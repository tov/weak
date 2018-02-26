cmake_minimum_required(VERSION 3.3)
project(intersections CXX)

include_directories(src)
include_directories(3rd_party)

macro (add_executable17 name)
    add_executable(${name} ${ARGN})
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED On)
endmacro (add_executable17)

add_executable17(weak_hash_table_test
        test/catch_main.cpp
        test/wus_test.cpp
        src/weak_unordered_set.h
        src/raw_vector.h)

add_executable17(raw_vector_test
        test/catch_main.cpp
        test/raw_vector_test.cpp
        src/raw_vector.h)
