#pragma once

#include "weak_unordered_set.h"

#include <memory>
#include <string>
#include <string_view>

namespace weak::example::intern_table {

class Symbol;

/// Interns a symbol in the default symbol table.
Symbol intern(std::string_view);

/// A table for interning symbols.
class Symbol_table
{
private:
    // We store (weak_ptrs to) strings, but we want to look them up by
    // std::string_view, so we use a hasher and equality for that.
    weak::weak_unordered_set<
            std::string,
            std::hash<std::string_view>,
            std::equal_to<std::string_view>
    > table_;

public:
    Symbol_table() = default;

    Symbol_table(const Symbol_table&) = delete;
    Symbol_table& operator=(const Symbol_table&) = delete;

    Symbol_table(Symbol_table&&) = default;
    Symbol_table& operator=(Symbol_table&&) = default;

    /// Interns a symbol in the table, returning the same pointer for the
    /// same symbol (if observable).
    Symbol intern(std::string_view);
};

/// A symbol is an interned string, which means they can be compared by
/// comparing the pointer.
class Symbol
{
private:
    using repr_t = std::shared_ptr<const std::string>;

    repr_t ptr_;

    explicit Symbol(const repr_t&);

    friend class Symbol_table;

public:
    /// Constructs an uninterned symbol, which does not compare equal to any
    /// other symbol.
    static Symbol uninterned(std::string_view);

    /// The name of the symbol.
    std::string_view name() const;

    /// Symbol equality.
    bool operator==(const Symbol& other) const;

    Symbol(const Symbol&) = default;
    Symbol(Symbol&&) = default;
    Symbol& operator=(const Symbol&) = default;
    Symbol& operator=(Symbol&&) = default;
};

/// Symbol disequality.
bool operator!=(const Symbol&, const Symbol&);

} // end namespace weak::example::intern_table
