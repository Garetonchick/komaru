#include "type.hpp"

#include <format>
#include <cassert>

#include <komaru/util/std_extensions.hpp>

namespace komaru::lang {

std::deque<Type::Variant>& Type::GetStorage() {
    static std::deque<Type::Variant> storage;
    return storage;
}

std::unordered_map<std::string, Type::Variant*>& Type::GetIndex() {
    static std::unordered_map<std::string, Type::Variant*> index;
    return index;
}

template <typename T>
Type::Variant* Type::MakeType(const T& type) {
    auto it = GetIndex().find(type.GetID());
    if (it != GetIndex().end()) {
        return it->second;
    }

    Variant* new_type = &GetStorage().emplace_back(type);
    GetIndex().emplace(std::move(type.GetID()), new_type);
    return new_type;
}

Type::Type(const CommonType& type) {
    type_ = MakeType(type);
}

Type::Type(const TupleType& type) {
    type_ = MakeType(type);
}

Type::Type(const FunctionType& type) {
    type_ = MakeType(type);
}

Type::Type(const ListType& type) {
    type_ = MakeType(type);
}

Type::Type(const Variant* type)
    : type_(type) {
}

const Type::Variant* Type::GetVariantPointer() const {
    return type_;
}

Type Type::Simple(std::string name) {
    assert(!name.empty() && IsConcreteTypeName(name) && std::isalpha(name[0]));
    return Parameterized(std::move(name), {});
}

Type Type::Parameterized(std::string name, std::vector<Type> params) {
    auto id = CommonType::MakeID(name, params);
    auto it = GetIndex().find(id);
    if (it != GetIndex().end()) {
        return Type(it->second);
    }

    Variant* new_type = &GetStorage().emplace_back(CommonType(std::move(name), std::move(params)));
    GetIndex().emplace(std::move(id), new_type);

    return Type(new_type);
}

Type Type::Tuple(std::vector<Type> types) {
    auto id = TupleType::MakeID(types);
    auto it = GetIndex().find(id);
    if (it != GetIndex().end()) {
        return Type(it->second);
    }

    Variant* new_type = &GetStorage().emplace_back(TupleType(std::move(types)));
    GetIndex().emplace(std::move(id), new_type);

    return Type(new_type);
}

Type Type::TupleFromSimples(std::vector<std::string> names) {
    std::vector<Type> types;
    types.reserve(names.size());

    for (auto& name : names) {
        types.emplace_back(Simple(std::move(name)));
    }

    return Tuple(std::move(types));
}

Type Type::Function(Type source, Type target) {
    std::string key = FunctionType::MakeID(source, target);
    auto it = GetIndex().find(key);
    if (it != GetIndex().end()) {
        return Type(it->second);
    }

    Variant* new_type = &GetStorage().emplace_back(FunctionType(source, target));
    GetIndex().emplace(std::move(key), new_type);

    return Type(new_type);
}

// Function with multiple parameters
Type Type::FunctionChain(std::span<Type> types) {
    if (types.empty()) {
        return Type::Singleton();
    }

    if (types.size() == 1) {
        return types[0];
    }

    Type target = FunctionChain(types.subspan(1));

    return Function(types[0], target);
}

Type Type::List(Type inner_type) {
    std::string key = ListType::MakeID(inner_type);
    auto it = GetIndex().find(key);
    if (it != GetIndex().end()) {
        return Type(it->second);
    }

    Variant* new_type = &GetStorage().emplace_back(ListType(inner_type));
    GetIndex().emplace(std::move(key), new_type);

    return Type(new_type);
}

Type Type::Var(std::string name) {
    assert(!name.empty() && !IsConcreteTypeName(name) && std::isalpha(name[0]));
    return Type::Parameterized(std::move(name), {});
}

Type Type::Auto() {
    return Type::Var("auto");
}

Type Type::Singleton() {
    return Type::Simple("S");
}

Type Type::Int() {
    return Type::Simple("Int");
}

Type Type::Float() {
    return Type::Simple("Float");
}

Type Type::Double() {
    return Type::Simple("Double");
}

Type Type::Char() {
    return Type::Simple("Char");
}

Type Type::Bool() {
    return Type::Simple("Bool");
}

Type Type::String() {
    return Type::List(Type::Char());
}

const std::string& Type::GetName() const {
    return this->Visit([](const TypeLike auto& t) -> const std::string& {
        return t.GetName();
    });
}

bool Type::IsConcrete() const {
    return this->Visit([](const TypeLike auto& t) -> bool {
        return t.IsConcrete();
    });
}

std::uintptr_t Type::GetID() const {
    return reinterpret_cast<std::uintptr_t>(type_);
}

Type Type::Pow(size_t n) const {
    return Tuple(std::vector<Type>(n, *this));
}

size_t Type::GetComponentsNum() const {
    return this->Visit(util::Overloaded{[](const TupleType& t) -> size_t {
                                            return t.GetTypesNum();
                                        },
                                        [](const TypeLike auto&) -> size_t {
                                            return 1;
                                        }});
}

bool Type::operator==(Type o) const {
    // Because each type in storage is unique and never changes it's location
    // we can just compare pointers
    return type_ == o.type_;
}

CommonType::CommonType(std::string main_name, std::vector<Type> params)
    : main_name_(std::move(main_name)),
      params_(std::move(params)) {
    assert(!main_name_.empty());

    name_ = main_name_;
    for (const auto& param : params_) {
        name_ += " " + std::string(param.GetName());
    }
}

const std::string& CommonType::GetName() const {
    return name_;
}

bool CommonType::IsConcrete() const {
    if (std::islower(main_name_.front())) {
        return false;
    }

    for (const auto& param : params_) {
        if (!param.IsConcrete()) {
            return false;
        }
    }
    return true;
}

std::string CommonType::GetID() const {
    return MakeID(name_, params_);
}

const std::string& CommonType::GetMainName() const {
    return main_name_;
}

const std::vector<Type>& CommonType::GetParams() const {
    return params_;
}

bool CommonType::HasParams() const {
    return !params_.empty();
}

bool CommonType::operator==(const CommonType& o) const {
    return name_ == o.name_ && util::VecEq(params_, o.params_);
}

std::string CommonType::MakeID(const std::string& name, const std::vector<Type>& params) {
    std::string id = name;
    for (const auto& param : params) {
        id += "_" + std::to_string(param.GetID());
    }
    return id;
}

TupleType::TupleType(std::vector<Type> inner_types)
    : inner_types_(std::move(inner_types)) {
    for (size_t i = 0; i < inner_types_.size(); ++i) {
        name_ += inner_types_[i].GetName();
        if (i + 1 != inner_types_.size()) {
            name_ += " x ";
        }
    }
}

const std::string& TupleType::GetName() const {
    return name_;
}

bool TupleType::IsConcrete() const {
    for (const auto& type : inner_types_) {
        if (!type.IsConcrete()) {
            return false;
        }
    }
    return true;
}

const std::vector<Type>& TupleType::GetTupleTypes() const {
    return inner_types_;
}

size_t TupleType::GetTypesNum() const {
    return inner_types_.size();
}

std::string TupleType::GetID() const {
    return MakeID(inner_types_);
}

bool TupleType::operator==(const TupleType& o) const {
    if (inner_types_.size() != o.inner_types_.size()) {
        return false;
    }
    for (size_t i = 0; i < inner_types_.size(); ++i) {
        if (inner_types_[i] != o.inner_types_[i]) {
            return false;
        }
    }
    return true;
}

std::string TupleType::MakeID(const std::vector<Type>& types) {
    std::string id = "(";
    for (size_t i = 0; i < types.size(); ++i) {
        id += std::to_string(types[i].GetID());
        if (i + 1 != types.size()) {
            id += ",";
        }
    }
    return id + ")";
}

FunctionType::FunctionType(Type source, Type target)
    : source_(source),
      target_(target) {
    name_ = std::format("{} -> {}", Source().GetName(), Target().GetName());
}

const std::string& FunctionType::GetName() const {
    return name_;
}

bool FunctionType::IsConcrete() const {
    return Source().IsConcrete() && Target().IsConcrete();
}

std::string FunctionType::GetID() const {
    return MakeID(Source(), Target());
}

Type FunctionType::Source() const {
    return source_;
}

Type FunctionType::Target() const {
    return target_;
}

bool FunctionType::operator==(const FunctionType& o) const {
    return Source() == o.Source() && Target() == o.Target();
}

std::string FunctionType::MakeID(Type source, Type target) {
    return std::format("{} -> {}", source.GetID(), target.GetID());
}

ListType::ListType(Type inner_type)
    : inner_type_(inner_type),
      name_(std::format("[{}]", inner_type.GetName())) {
}

const std::string& ListType::GetName() const {
    return name_;
}

bool ListType::IsConcrete() const {
    return inner_type_.IsConcrete();
}

std::string ListType::GetID() const {
    return MakeID(inner_type_);
}

Type ListType::Inner() const {
    return inner_type_;
}

bool ListType::operator==(const ListType& o) const {
    return inner_type_ == o.inner_type_;
}

std::string ListType::MakeID(Type inner_type) {
    return std::format("[{}]", inner_type.GetID());
}

Type operator*(Type t1, Type t2) {
    bool tup1 = t1.Holds<TupleType>();
    bool tup2 = t2.Holds<TupleType>();
    if (!tup1 && !tup2) {
        return Type::Tuple({t1, t2});
    }
    std::vector<Type> types;

    if (tup1) {
        types.insert_range(types.end(), t1.GetVariant<TupleType>().GetTupleTypes());
    } else {
        types.push_back(t1);
    }
    if (tup2) {
        types.insert_range(types.end(), t2.GetVariant<TupleType>().GetTupleTypes());
    } else {
        types.push_back(t2);
    }

    return Type::Tuple(types);
}

bool IsConcreteTypeName(const std::string& name) {
    return !name.empty() && std::isupper(name.front());
}

}  // namespace komaru::lang
