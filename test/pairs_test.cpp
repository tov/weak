#include "weak_pair.h"
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

    CHECK( !pair.expired() );
    CHECK( pair.lock().first == hello );
    CHECK( pair.lock().second == 5 );

    hello = nullptr;

    CHECK( pair.expired() );
    CHECK( pair.lock().first == nullptr );
}
