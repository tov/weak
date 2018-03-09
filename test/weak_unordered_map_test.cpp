#include "weak_weak_unordered_map.h"
#include "weak_key_unordered_map.h"
#include "weak_value_unordered_map.h"

#include <catch.hpp>
#include <string>

using namespace weak;
using namespace std;

TEST_CASE("weak_key_unordered_map")
{
    weak_key_unordered_map<int, int> map;
    CHECK( !map.member(5) );
    CHECK( map.find(5) == map.end() );

    auto five = make_shared<int>(5);
    map.insert({five, 5});
    CHECK( map.find(5) != map.end() );

    five = nullptr;
    CHECK( map.find(5) == map.end() );

    five = make_shared<int>(5);
    map[five] = 5;
    CHECK( map.find(5) != map.end() );
    CHECK( map[five] == 5 );

    CHECK( map == map );
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

    CHECK( map == map );
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

    CHECK( map == map );
}

TEST_CASE("copy")
{
    weak_key_unordered_map<string, int> map;

    auto hello = make_shared<string>("hello");
    map[hello] = 5;

    weak_key_unordered_map<string, int> copy_map(map);
    // copy_map = map;

    CHECK( copy_map == map );
    CHECK( map == copy_map );
    CHECK( copy_map == copy_map );
}
