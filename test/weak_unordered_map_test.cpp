#include "weak_unordered_map.h"
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
}

TEST_CASE("weak_value_unordered_map")
{
    weak_value_unordered_map<string, int> map;
}

TEST_CASE("weak_unordered_map")
{
    weak_unordered_map<string, int> map;
}