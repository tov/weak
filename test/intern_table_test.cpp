#include "intern_table.h"
#include <catch.hpp>

using namespace weak::example::intern_table;

TEST_CASE("uninterned symbol")
{
    auto a1 = Symbol::uninterned("a");
    auto a2 = Symbol::uninterned("a");

    CHECK( a1.name() == "a" );
    CHECK( a2.name() == "a" );

    CHECK( a1 == a1 );
    CHECK( a2 == a2 );
    CHECK( a1 != a2 );
}

TEST_CASE("interned symbols")
{
    auto a1 = intern("a");
    auto a2 = intern("a");
    auto b1 = intern("b");
    auto b2 = intern("b");

    CHECK( a1.name() == "a" );
    CHECK( a2.name() == "a" );
    CHECK( b1.name() == "b" );
    CHECK( b2.name() == "b" );

    CHECK( a1 == a2 );
    CHECK( b1 == b2 );
    CHECK( a1 != b1 );
    CHECK( a1 != b2 );
    CHECK( a2 != b1 );
    CHECK( a2 != b2 );
}
