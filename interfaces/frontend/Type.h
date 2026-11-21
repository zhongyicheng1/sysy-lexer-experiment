#pragma once

#include <utility>
#include <vector>

// Base types supported by SysY.  VOID is also used for the default-constructed
// Type so that an unset type is never mistaken for a valid expression type.
enum class BaseType {
    INT,
    BOOL,
    VOID,
    FLOAT,
};

// A complete SysY type: a base type plus zero or more array dimensions.
// For example, "int a[2][3]" is {BaseType::INT, {2, 3}}.
struct Type {
    BaseType base = BaseType::VOID;
    std::vector<int> dimensions;

    Type() = default;

    explicit Type(BaseType baseType) : base(baseType) {}

    Type(BaseType baseType, std::vector<int> dims)
        : base(baseType), dimensions(std::move(dims)) {}

    bool isVoid() const noexcept { return base == BaseType::VOID; }
    bool isInt() const noexcept { return base == BaseType::INT; }
    bool isBool() const noexcept { return base == BaseType::BOOL; }
    bool isFloat() const noexcept { return base == BaseType::FLOAT; }

    // Scalars have no array dimensions; VOID is not a value type.
    bool isScalar() const noexcept {
        return dimensions.empty() && base != BaseType::VOID;
    }

    bool isArray() const noexcept { return !dimensions.empty(); }

    bool operator==(const Type& rhs) const {
        return base == rhs.base && dimensions == rhs.dimensions;
    }

    bool operator!=(const Type& rhs) const { return !(*this == rhs); }
};
