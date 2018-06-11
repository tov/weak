#include "weak_unordered_set.h"
#include <catch.hpp>
#include <memory>
#include <string>

using namespace std;
using namespace weak;

using string_ptr_set = weak_hash_table_base<by_ptr<weak_ptr<string>>>;

TEST_CASE("initialize empty")
{
    string_ptr_set set;
    auto p1 = make_shared<string>("hello");
    set.insert(p1);
}
