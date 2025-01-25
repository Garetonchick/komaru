#pragma once

#include <util/non_copyable_non_movable.hpp>

#include <string>
#include <vector>


namespace komaru::lang {

enum class BuiltinType {
    Int,
    Char,
    Float,
    Tuple,
    List,
    Generic,
};

class TypeRegistry;

class Type : util::NonCopyable {
    friend TypeRegistry;
public:
    const std::string& GetName() const;
    BuiltinType GetBaseType() const;
    const std::vector<const Type*>& GetInnerTypes() const;

    bool operator==(const Type& o) const = delete; // Use IsSameAs
    bool IsSameAs(const Type& o) const;

private:
    Type(std::string name, BuiltinType type);
    Type(std::string name, BuiltinType base_type, std::vector<const Type*> inner_types);

private:
    std::string name_;
    BuiltinType base_type_;
    std::vector<const Type*> inner_types_;
};

}
