#include "weak_weak_unordered_map.h"
#include "weak_key_unordered_map.h"
#include "weak_value_unordered_map.h"

#include <catch.hpp>
#include <string>

using namespace weak;
using namespace std;

TEST_CASE("weak_key_unordered_map")
{
    weak_key_unordered_map<string, int> map;
    CHECK( map.find("hello") == map.end() );

    auto hello = make_shared<string>("hello");
    map.insert({hello, 5});
    CHECK( map.find("hello") != map.end() );

    hello = nullptr;
    CHECK( map.find("hello") == map.end() );

    hello = make_shared<string>("hello");
    map[hello] = 5;
    CHECK( map[hello] == 5 );
}

TEST_CASE("weak_value_unordered_map")
{
    weak_value_unordered_map<string, int> map;
    CHECK( map.find("hello") == map.end() );

    auto five = make_shared<int>(5);
    map.insert({"hello", five});
    CHECK( map.find("hello") != map.end() );

    five = nullptr;
    CHECK( map.find("hello") == map.end() );

    five = map["hello"] = make_shared<int>(5);
    CHECK( map.find("hello") != map.end() );
}

TEST_CASE("weak_unordered_map")
{
    weak_weak_unordered_map<string, int> map;
    CHECK( map.find("hello") == map.end() );

    auto hello = make_shared<string>("hello");
    auto five = make_shared<int>(5);
    map.insert({hello, five});
    CHECK( map.find("hello") != map.end() );

    five = nullptr;
    CHECK( map.find("hello") == map.end() );

    five = map[hello] = make_shared<int>(5);
    CHECK( map.find("hello") != map.end() );
}
