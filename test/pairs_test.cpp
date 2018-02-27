#include "weak_weak_pair.h"
#include "weak_key_pair.h"
#include "weak_value_pair.h"

#include <catch.hpp>
#include <memory>
#include <string>

using namespace weak;
using namespace std;

TEST_CASE("weak_key_pair")
{
    auto hello = make_shared<string>("hello");
    weak_key_pair<string, int> pair{hello, 5};

    REQUIRE( !pair.expired() );
    CHECK( pair.lock().first == hello );
    CHECK( pair.lock().second == 5 );

    hello = nullptr;

    CHECK( pair.expired() );
    CHECK( pair.lock().first == nullptr );
}

TEST_CASE("weak_value_pair")
{
    auto hello = make_shared<string>("hello");
    weak_value_pair<int, string> pair{5, hello};

    CHECK( !pair.expired() );

    {
        auto locked = pair.lock();
        hello = nullptr;

        REQUIRE( !pair.expired() );
        CHECK(pair.lock().first == 5);
        CHECK(*pair.lock().second == "hello");
    }

    CHECK( pair.expired() );
    CHECK( pair.lock().second == nullptr );
}

TEST_CASE("weak_pair")
{
    auto hello = make_shared<string>("hello");
    auto world = make_shared<string>("world");
    weak_weak_pair<string, string> pair{hello, world};
    CHECK( !pair.expired() );

    hello = nullptr;
    CHECK( pair.expired() );

    hello = make_shared<string>("hello");
    pair = {hello, world};
    CHECK( !pair.expired() );

    world = nullptr;
    CHECK( pair.expired() );
}

