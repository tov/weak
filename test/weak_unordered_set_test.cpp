#include "weak_unordered_set.h"

#include <catch.hpp>

#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_set>

using namespace std;
using namespace weak;

TEST_CASE("Default construction and insertion")
{
    weak_unordered_set<int> set;

    auto five = make_shared<int>(5);
    set.insert(five);

    CHECK( set.member(5) );
    CHECK_FALSE( set.member(6) );

    vector<int> actual;
    for (auto ptr : set)
        actual.push_back(*ptr);
    CHECK( actual == vector{5} );

    five = nullptr;

    CHECK_FALSE( set.member(5) );
    CHECK_FALSE( set.member(6) );
}

TEST_CASE("growing")
{
    vector<shared_ptr<int>> holder;
    weak_unordered_set<int> set;

    for (size_t i = 0; i < 1000; ++i) {
        auto new_ptr = make_shared<int>(i);
        holder.push_back(new_ptr);
        set.insert(new_ptr);
    }

    CHECK( 1000 == set.size() );
    CHECK( set.member(999) );
    holder.pop_back();
    CHECK( 1000 == set.size() );
    CHECK( !set.member(999) );

    size_t count = 0;
    for (const auto& ptr : set) ++count;
    CHECK( count == 999 );

    CHECK( 1000 == set.size() );

    CHECK( set == set );
    CHECK( set != weak_unordered_set<int>{} );
}

class SetTester
{
public:
    void insert(int v)
    {
        auto ptr = make_shared<int>(v);
        holder_.insert(ptr);
        set_.insert(ptr);
    }

    void forget(int v)
    {
        auto iter = std::find_if(holder_.begin(), holder_.end(),
                                 [&](auto ptr) { return *ptr == v; });
        if (iter != holder_.end()) holder_.erase(iter);
    }

    void erase(int v)
    {
        set_.erase(v);
        forget(v);
    };

    bool member(int v) const
    {
        return set_.member(v);
    }

    unordered_set<int> members() const
    {
        unordered_set<int> result;
        for (const auto& ptr : holder_) result.insert(*ptr);
        return result;
    }

private:
    using ptr_t = shared_ptr<int>;

    struct Hash
    {
        size_t operator()(int v) const
        {
            return size_t(v) / 10 * 5;
        }

        size_t operator()(const ptr_t& ptr) const
        {
            return operator()(*ptr);
        }
    };

    struct EqualTo
    {
        bool operator()(const ptr_t& a, const ptr_t& b) const
        {
            return *a == *b;
        }
    };

    unordered_set<shared_ptr<int>, Hash, EqualTo> holder_;
    weak_unordered_set<int, Hash> set_;
};

TEST_CASE("erase")
{
    SetTester tester;

    for (int z = 0; z < 20; ++z) {
        tester.insert(z);
    }

    for (int z : tester.members()) {
        CHECK( tester.member(z) );
    }

    tester.erase(0);

    for (int z : tester.members()) {
        CHECK( tester.member(z) );
    }

    tester.forget(3);

    for (int z : tester.members()) {
        CHECK( tester.member(z) );
    }

    tester.forget(4);
    tester.forget(5);
    tester.erase(1);

    for (int z : tester.members()) {
        CHECK( tester.member(z) );
    }
}
