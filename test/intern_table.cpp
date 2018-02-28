#include "intern_table.h"

const std::string& Symbol::name() const
{
    return *ptr_;
}

bool Symbol::operator==(const Symbol& other) const
{
    return ptr_ == other.ptr_;
}

Symbol::Symbol(std::shared_ptr<const std::string> ptr)
        : ptr_(std::move(ptr))
{ }

Symbol Symbol::uninterned(const std::string& name)
{
    return Symbol(std::make_shared<const std::string>(name));
}

bool operator!=(const Symbol& a, const Symbol& b)
{
    return !(a == b);
}

Symbol intern(const std::string& name)
{
    static Symbol_table table;
    return table.intern(name);
}

Symbol Symbol_table::intern(const std::string& name)
{
    auto iter = table_.find(name);
    if (iter == table_.end()) {
        auto ptr = std::make_shared<const std::string>(name);
        table_.insert(ptr);
        return Symbol(ptr);
    } else {
        return Symbol(*iter);
    }
}

