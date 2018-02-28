#pragma once

#include "weak_unordered_set.h"

#include <memory>
#include <string>

/// A symbol is an interned string, which means they can be compared by
/// comparing the pointer.
class Symbol
{
public:
    /// The name of the symbol.
    const std::string& name() const;

    /// Symbol equality.
    bool operator==(const Symbol& other) const;

    /// Constructs an uninterned symbol, which does not compare equal to any
    /// other symbol.
    static Symbol uninterned(const std::string&);

    Symbol(const Symbol&) = default;
    Symbol(Symbol&&) = default;
    Symbol& operator=(const Symbol&) = default;
    Symbol& operator=(Symbol&&) = default;

private:
    std::shared_ptr<const std::string> ptr_;

    explicit Symbol(std::shared_ptr<const std::string> ptr);

    friend class Symbol_table;
};

/// Symbol disequality.
bool operator!=(const Symbol&, const Symbol&);

/// A table for interning symbols.
class Symbol_table
{
public:
    Symbol_table() = default;

    Symbol_table(const Symbol_table&) = delete;
    Symbol_table& operator=(const Symbol_table&) = delete;

    Symbol_table(Symbol_table&&) = default;
    Symbol_table& operator=(Symbol_table&&) = default;

    /// Interns a symbol in the table, returning the same pointer for the
    /// same symbol (if observable).
    Symbol intern(const std::string&);

private:
    weak::weak_unordered_set<std::string> table_;
};

/// Interns a symbol in the default symbol table.
Symbol intern(const std::string&);
