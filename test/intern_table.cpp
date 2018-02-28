#include "intern_table.h"

Symbol intern(std::string_view name)
{
    static Symbol_table table;
    return table.intern(name);
}

Symbol Symbol_table::intern(std::string_view name)
{
    // Check whether the name is found in the table. If we get back a shared_ptr
    // then we wrap that as a symbol and return it. Otherwise, we have to allocate
    // the shared_ptr for symbol and store it in the table before returning it.
    auto iter = table_.find(name);
    if (iter != table_.end()) {
        return Symbol(*iter);
    } else {
        auto ptr = std::make_shared<const std::string>(name);
        table_.insert(ptr);
        return Symbol(ptr);
    }
}

Symbol::Symbol(const repr_t& ptr) : ptr_(ptr)
{ }

Symbol Symbol::uninterned(std::string_view name)
{
    return Symbol(std::make_shared<const std::string>(name));
}

std::string_view Symbol::name() const
{
    return *ptr_;
}

bool Symbol::operator==(const Symbol& other) const
{
    return ptr_ == other.ptr_;
}

bool operator!=(const Symbol& a, const Symbol& b)
{
    return !(a == b);
}

