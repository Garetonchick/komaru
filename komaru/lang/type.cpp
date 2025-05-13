#include "type.hpp"

#include <format>
#include <cassert>

#include <komaru/util/std_extensions.hpp>
#include <komaru/util/string.hpp>

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

Type Type::Common(std::string name) {
    return Parameterized(std::move(name), {});
}

Type Type::Simple(std::string name) {
    assert(!name.empty() && IsConcreteTypeName(name) && std::isalpha(name[0]));
    return Parameterized(std::move(name), {});
}

Type Type::Parameterized(std::string name, std::vector<Type> params) {
    if (name == "[]") {
        assert(params.size() == 1);
        return Type::List(params[0]);
    }

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
Type FunctionChainImpl(std::span<const Type> types) {
    if (types.empty()) {
        return Type::Singleton();
    }

    if (types.size() == 1) {
        return types[0];
    }

    Type target = FunctionChainImpl(types.subspan(1));

    return Type::Function(types[0], target);
}

Type Type::FunctionChain(const std::vector<Type>& types) {
    return FunctionChainImpl(types);
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

std::string Type::ToString(Style style) const {
    return this->Visit([style](const TypeLike auto& t) -> std::string {
        return t.ToString(style);
    });
}

bool Type::IsConcrete() const {
    return this->Visit([](const TypeLike auto& t) -> bool {
        return t.IsConcrete();
    });
}

bool Type::ShouldBeShielded() const {
    return this->Visit([](const TypeLike auto& t) -> bool {
        return t.ShouldBeShielded();
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

std::vector<Type> Type::GetComponents() const {
    return this->Visit(util::Overloaded{[](const TupleType& t) -> std::vector<Type> {
                                            return t.GetTupleTypes();
                                        },
                                        [this](const TypeLike auto&) -> std::vector<Type> {
                                            return {*this};
                                        }});
}

size_t Type::GetParamNum() const {
    return this->Visit(util::Overloaded{[](const FunctionType& t) -> size_t {
                                            return t.GetParamNum();
                                        },
                                        [](const TypeLike auto&) -> size_t {
                                            return 0;
                                        }});
}

bool Type::IsValueType() const {
    return this->Visit(util::Overloaded{[](const FunctionType& t) -> bool {
                                            return t.IsValueType();
                                        },
                                        [](const TypeLike auto&) -> bool {
                                            return true;
                                        }});
}

std::vector<Type> Type::FlattenFunction() const {
    return this->Visit(util::Overloaded{[](const FunctionType& t) -> std::vector<Type> {
                                            std::vector<Type> types;
                                            if (t.Source() != Type::Singleton()) {
                                                types.push_back(t.Source());
                                            }
                                            types.insert_range(types.end(),
                                                               t.Target().FlattenFunction());
                                            return types;
                                        },
                                        [this](const TypeLike auto&) -> std::vector<Type> {
                                            return {*this};
                                        }});
}

bool Type::IsTypeVar() const {
    return Holds<CommonType>() && !GetVariant<CommonType>().HasTypeParams() && !IsConcrete();
}

size_t Type::TypeVariantIndex() const {
    return type_->index();
}

Type Type::Pure() const {
    std::vector<Type> types = FlattenFunction();
    if (types.empty()) {
        return Type::Singleton();
    }
    if (types.size() == 1) {
        return types[0];
    }
    return Type::FunctionChain(types);
}

bool Type::operator==(Type o) const {
    // Because each type in storage is unique and never changes it's location
    // we can just compare pointers
    return type_ == o.type_;
}
bool Type::operator<(Type o) const {
    return type_ < o.type_;
}

TypeConstructor::TypeConstructor(std::string name, size_t num_params)
    : name_(std::move(name)),
      num_params_(num_params) {
}

std::string TypeConstructor::ToString(Style) const {
    return name_;
}

size_t TypeConstructor::GetNumParams() const {
    return num_params_;
}

bool TypeConstructor::operator==(const TypeConstructor& o) const {
    return name_ == o.name_ && num_params_ == o.num_params_;
}

bool TypeConstructor::operator<(const TypeConstructor& o) const {
    if (num_params_ != o.num_params_) {
        return num_params_ < o.num_params_;
    }
    return name_ < o.name_;
}

CommonType::CommonType(std::string name, std::vector<Type> params)
    : name_(std::move(name)),
      params_(std::move(params)) {
    assert(!name_.empty());
}

std::string CommonType::ToString(Style style) const {
    if (style == Style::Debug) {
        return ToStringDebug();
    }
    if (style == Style::Haskell && name_ == "S") {
        return "()";
    }
    if (style == Style::Haskell && name_ == "Str") {
        return "String";
    }

    std::string res = name_;

    for (const auto& param : params_) {
        res += " " + util::Shield(param.ToString(style), param.ShouldBeShielded());
    }

    return res;
}

bool CommonType::IsConcrete() const {
    if (std::islower(name_.front())) {
        return false;
    }

    for (const auto& param : params_) {
        if (!param.IsConcrete()) {
            return false;
        }
    }
    return true;
}

bool CommonType::ShouldBeShielded() const {
    return !params_.empty();
}

std::string CommonType::GetID() const {
    return MakeID(name_, params_);
}

const std::string& CommonType::GetName() const {
    return name_;
}

const std::vector<Type>& CommonType::GetTypeParams() const {
    return params_;
}

size_t CommonType::NumTypeParams() const {
    return params_.size();
}

bool CommonType::HasTypeParams() const {
    return !params_.empty();
}

bool CommonType::IsAuto() const {
    return name_ == "auto";
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

std::string CommonType::ToStringDebug() const {
    std::string res;
    res += "Common{" + name_ + ":";
    for (const auto& param : params_) {
        res += " " + param.ToString(Style::Debug);
    }
    res += "}";
    return res;
}

TupleType::TupleType(std::vector<Type> inner_types)
    : inner_types_(std::move(inner_types)) {
}

std::string TupleType::ToString(Style style) const {
    switch (style) {
        case Style::Komaru:
            return ToStringKomaru();
        case Style::Haskell:
            return ToStringHaskell();
        case Style::Debug:
            return ToStringDebug();
    }
}

bool TupleType::IsConcrete() const {
    for (const auto& type : inner_types_) {
        if (!type.IsConcrete()) {
            return false;
        }
    }
    return true;
}

bool TupleType::ShouldBeShielded() const {
    return false;
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

std::string TupleType::ToStringKomaru() const {
    std::string res;
    for (size_t i = 0; i < inner_types_.size(); ++i) {
        res += util::Shield(inner_types_[i].ToString(Style::Komaru),
                            inner_types_[i].ShouldBeShielded());
        if (i + 1 != inner_types_.size()) {
            res += " x ";
        }
    }
    return res;
}

std::string TupleType::ToStringHaskell() const {
    std::string res = "(";
    for (size_t i = 0; i < inner_types_.size(); ++i) {
        res += inner_types_[i].ToString(Style::Haskell);
        if (i + 1 != inner_types_.size()) {
            res += ", ";
        }
    }
    return res + ")";
}

std::string TupleType::ToStringDebug() const {
    std::string res = "Tuple{";
    for (size_t i = 0; i < inner_types_.size(); ++i) {
        res += inner_types_[i].ToString(Style::Debug);
        if (i + 1 != inner_types_.size()) {
            res += ", ";
        }
    }
    return res + "}";
}

FunctionType::FunctionType(Type source, Type target)
    : source_(source),
      target_(target) {
}

std::string FunctionType::ToString(Style style) const {
    if (style == Style::Haskell && Source() == Type::Singleton()) {
        return Target().ToString(style);
    }

    std::string source_str = util::Shield(Source().ToString(style), Source().ShouldBeShielded());
    std::string target_str = Target().ToString(style);

    return std::format("{} -> {}", source_str, target_str);
}

bool FunctionType::IsConcrete() const {
    return Source().IsConcrete() && Target().IsConcrete();
}

bool FunctionType::ShouldBeShielded() const {
    return true;
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

size_t FunctionType::GetParamNum() const {
    if (Source() == Type::Singleton()) {
        return 0;
    }
    return 1 + target_.GetParamNum();
}

bool FunctionType::IsValueType() const {
    return Source() == Type::Singleton();
}

bool FunctionType::operator==(const FunctionType& o) const {
    return Source() == o.Source() && Target() == o.Target();
}

std::string FunctionType::MakeID(Type source, Type target) {
    return std::format("{} -> {}", source.GetID(), target.GetID());
}

ListType::ListType(Type inner_type)
    : inner_type_(inner_type) {
}

std::string ListType::ToString(Style style) const {
    return std::format("[{}]", inner_type_.ToString(style));
}

bool ListType::IsConcrete() const {
    return inner_type_.IsConcrete();
}

bool ListType::ShouldBeShielded() const {
    return false;
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

[[nodiscard]] bool MergeMatchMaps(MatchMap& mapping, const MatchMap& sub_mapping) {
    for (const auto& [type_var_name, type_or_constructor] : sub_mapping) {
        auto it = mapping.find(type_var_name);
        if (it != mapping.end()) {
            if (it->second != type_or_constructor) {
                return false;
            }
        } else {
            mapping.emplace(type_var_name, type_or_constructor);
        }
    }
    return true;
}

std::optional<Type> TryDeduceTypes(Type func_type, Type arg_type) {
    return TryDeduceTypes(func_type, std::map<size_t, Type>{{0, arg_type}});
}

std::optional<Type> TryDeduceTypes(Type func_type, const std::map<size_t, Type>& arg_mapping) {
    if (func_type.IsValueType()) {
        if (arg_mapping.size() == 1 && arg_mapping.begin()->second == Type::Singleton()) {
            return func_type;
        }
        return std::nullopt;
    }

    if (!func_type.Holds<FunctionType>()) {
        return std::nullopt;
    }

    auto types = func_type.FlattenFunction();
    MatchMap match_map;

    for (size_t i = 0; i < types.size(); ++i) {
        auto it = arg_mapping.find(i);
        if (it != arg_mapping.end()) {
            auto maybe_mapping = TryMatchTypes(types[i], it->second);
            if (!maybe_mapping) {
                return std::nullopt;
            }

            if (!MergeMatchMaps(match_map, maybe_mapping.value())) {
                return std::nullopt;
            }
        }
    }

    return ApplyMatchMap(func_type, match_map);
}

Type DeduceTypes(Type func_type, Type arg_type) {
    if (auto deduced = TryDeduceTypes(func_type, arg_type)) {
        return deduced.value();
    }

    return func_type;
}

Type DeduceTypes(Type func_type, const std::map<size_t, Type>& arg_mapping) {
    if (auto deduced = TryDeduceTypes(func_type, arg_mapping)) {
        return deduced.value();
    }

    return func_type;
}

std::optional<Type> TryMakeSubstitution(Type func_type, const std::map<size_t, Type>& arg_mapping) {
    if (func_type.IsValueType()) {
        if (arg_mapping.size() == 1 && arg_mapping.begin()->second == Type::Singleton()) {
            return func_type.FlattenFunction()[0];
        }
        return std::nullopt;
    }

    auto maybe_func_type = TryDeduceTypes(func_type, arg_mapping);
    if (!maybe_func_type) {
        return std::nullopt;
    }

    func_type = maybe_func_type.value();

    auto types = func_type.FlattenFunction();
    std::vector<Type> new_types;

    for (size_t i = 0; i + 1 < types.size(); ++i) {
        if (!arg_mapping.contains(i)) {
            new_types.push_back(types[i]);
        }
    }

    new_types.push_back(types.back());

    if (arg_mapping.size() + new_types.size() != types.size()) {
        return std::nullopt;
    }

    Type new_type = Type::FunctionChain(new_types);
    return new_type;
}

Type MakeSubstitution(Type func_type, const std::map<size_t, Type>& arg_mapping) {
    if (func_type.IsValueType()) {
        return func_type.FlattenFunction()[0];
    }

    func_type = DeduceTypes(func_type, arg_mapping);

    auto types = func_type.FlattenFunction();
    std::vector<Type> new_types;

    for (size_t i = 0; i + 1 < types.size(); ++i) {
        if (!arg_mapping.contains(i)) {
            new_types.push_back(types[i]);
        }
    }

    new_types.push_back(types.back());

    Type new_type = Type::FunctionChain(new_types);
    return new_type;
}

std::optional<MatchMap> TryMatchTypes(const CommonType& param_type, const CommonType& arg_type) {
    if (param_type.IsAuto()) {
        return MatchMap{};
    }

    if (param_type.NumTypeParams() != arg_type.NumTypeParams()) {
        return std::nullopt;
    }

    bool is_param_name_concrete = IsConcreteTypeName(param_type.GetName());
    bool is_arg_name_concrete = IsConcreteTypeName(arg_type.GetName());

    if (is_param_name_concrete && is_arg_name_concrete &&
        param_type.GetName() != arg_type.GetName()) {
        return std::nullopt;
    }

    MatchMap mapping;

    if (!is_param_name_concrete && is_arg_name_concrete) {
        mapping.emplace(param_type.GetName(),
                        TypeConstructor(arg_type.GetName(), arg_type.NumTypeParams()));
    }

    for (size_t i = 0; i < param_type.NumTypeParams(); ++i) {
        auto param_type_param = param_type.GetTypeParams()[i];
        auto arg_type_param = arg_type.GetTypeParams()[i];

        auto maybe_sub_mapping = TryMatchTypes(param_type_param, arg_type_param);

        if (!maybe_sub_mapping) {
            return std::nullopt;
        }

        auto sub_mapping = std::move(maybe_sub_mapping.value());

        if (!MergeMatchMaps(mapping, sub_mapping)) {
            return std::nullopt;
        }
    }

    return mapping;
}

std::optional<MatchMap> TryMatchTypes(const CommonType& param_type, const ListType& arg_type) {
    if (param_type.IsAuto()) {
        return MatchMap{};
    }

    if (IsConcreteTypeName(param_type.GetName())) {
        return std::nullopt;
    }

    if (param_type.NumTypeParams() == 0) {
        return MatchMap{{param_type.GetName(), lang::Type(arg_type)}};
    }
    if (param_type.NumTypeParams() == 1) {
        auto maybe_match_map = TryMatchTypes(param_type.GetTypeParams()[0], arg_type.Inner());
        if (!maybe_match_map) {
            return std::nullopt;
        }
        auto match_map = std::move(maybe_match_map.value());
        if (!MergeMatchMaps(match_map,
                            MatchMap{{param_type.GetName(), TypeConstructor("[]", 1)}})) {
            return std::nullopt;
        }
        return match_map;
    }

    return std::nullopt;
}

std::optional<MatchMap> TryMatchTypes(const TupleType& param_type, const TupleType& arg_type) {
    if (param_type.GetTypesNum() != arg_type.GetTypesNum()) {
        return std::nullopt;
    }

    MatchMap mapping;

    for (size_t i = 0; i < param_type.GetTypesNum(); ++i) {
        auto param_type_type = param_type.GetTupleTypes()[i];
        auto arg_type_type = arg_type.GetTupleTypes()[i];

        auto maybe_sub_mapping = TryMatchTypes(param_type_type, arg_type_type);

        if (!maybe_sub_mapping) {
            return std::nullopt;
        }

        if (!MergeMatchMaps(mapping, maybe_sub_mapping.value())) {
            return std::nullopt;
        }
    }

    return mapping;
}

std::optional<MatchMap> TryMatchTypes(const FunctionType& param_type,
                                      const FunctionType& arg_type) {
    if (param_type.GetParamNum() != arg_type.GetParamNum()) {
        return std::nullopt;
    }

    auto maybe_source_mapping = TryMatchTypes(param_type.Source(), arg_type.Source());
    if (!maybe_source_mapping) {
        return std::nullopt;
    }

    auto maybe_target_mapping = TryMatchTypes(param_type.Target(), arg_type.Target());
    if (!maybe_target_mapping) {
        return std::nullopt;
    }

    auto mapping = std::move(maybe_source_mapping.value());
    if (!MergeMatchMaps(mapping, maybe_target_mapping.value())) {
        return std::nullopt;
    }

    return mapping;
}

std::optional<MatchMap> TryMatchTypes(const ListType& param_type, const ListType& arg_type) {
    auto mapping = TryMatchTypes(param_type.Inner(), arg_type.Inner());
    if (!mapping) {
        return std::nullopt;
    }

    return mapping.value();
}

std::optional<MatchMap> TryMatchTypes(Type param_type, Type arg_type) {
    MatchMap mapping;

    if (param_type == Type::Auto() || arg_type.IsTypeVar()) {
        return mapping;
    }

    if (param_type.IsTypeVar()) {
        mapping.emplace(param_type.ToString(), arg_type);
        return mapping;
    }

    if (param_type.Holds<CommonType>() && arg_type.Holds<ListType>()) {
        return TryMatchTypes(param_type.GetVariant<CommonType>(), arg_type.GetVariant<ListType>());
    }

    if (param_type.TypeVariantIndex() != arg_type.TypeVariantIndex()) {
        return std::nullopt;
    }

    return param_type.Visit(
        [&]<TypeLike T>(const T& unboxed_param_type) -> std::optional<MatchMap> {
            assert(arg_type.Holds<T>());
            return TryMatchTypes(unboxed_param_type, arg_type.GetVariant<T>());
        });
}

MatchMap MatchTypes(Type param_type, Type arg_type) {
    auto mapping = TryMatchTypes(param_type, arg_type);
    if (!mapping) {
        return {};
    }

    return mapping.value();
}

Type ApplyMatchMap(const CommonType& type, const MatchMap& mapping) {
    std::string new_name = type.GetName();

    auto it = mapping.find(type.GetName());
    if (it != mapping.end()) {
        if (std::holds_alternative<Type>(it->second)) {
            assert(type.NumTypeParams() == 0);
            return std::get<Type>(it->second);
        } else {
            new_name = std::get<TypeConstructor>(it->second).ToString();
        }
    }

    std::vector<Type> new_params;
    for (const auto& param : type.GetTypeParams()) {
        new_params.push_back(ApplyMatchMap(param, mapping));
    }

    return Type::Parameterized(std::move(new_name), std::move(new_params));
}

Type ApplyMatchMap(const TupleType& type, const MatchMap& mapping) {
    std::vector<Type> new_types;
    for (const auto& param : type.GetTupleTypes()) {
        new_types.push_back(ApplyMatchMap(param, mapping));
    }

    return Type::Tuple(std::move(new_types));
}

Type ApplyMatchMap(const FunctionType& type, const MatchMap& mapping) {
    return Type::Function(ApplyMatchMap(type.Source(), mapping),
                          ApplyMatchMap(type.Target(), mapping));
}

Type ApplyMatchMap(const ListType& type, const MatchMap& mapping) {
    return Type::List(ApplyMatchMap(type.Inner(), mapping));
}

Type ApplyMatchMap(Type type, const MatchMap& mapping) {
    return type.Visit([&]<TypeLike T>(const T& unboxed_type) -> Type {
        return ApplyMatchMap(unboxed_type, mapping);
    });
}

bool CanBeSubstituted(Type param_type, Type arg_type, const MatchMap& mapping) {
    auto maybe_mapping = TryMatchTypes(param_type, arg_type);
    if (!maybe_mapping) {
        return false;
    }

    auto new_mapping = std::move(maybe_mapping.value());
    return MergeMatchMaps(new_mapping, mapping);
}

Type CurryFunction(Type source, Type target) {
    return source.Visit(util::Overloaded{[&](const TupleType& t) -> Type {
                                             std::vector<Type> new_types = t.GetTupleTypes();
                                             new_types.push_back(target);
                                             return Type::FunctionChain(new_types);
                                         },
                                         [&](const TypeLike auto&) -> Type {
                                             return Type::Function(source, target);
                                         }});
}

std::string ArgMappingToString(const std::map<size_t, Type>& arg_mapping) {
    std::string res;
    for (const auto& [i, type] : arg_mapping) {
        res += std::format("${}: {}; ", i, type.ToString());
    }
    return res;
}

std::optional<Type> FindCommonType(const std::vector<Type>& types) {
    if (types.empty()) {
        return Type::Var("a");
    }

    lang::Type type = types[0];

    for (lang::Type t : types) {
        auto maybe_match_map = TryMatchTypes(type, t);
        if (maybe_match_map.has_value()) {
            type = ApplyMatchMap(type, maybe_match_map.value());
            continue;
        }
        maybe_match_map = TryMatchTypes(t, type);
        if (!maybe_match_map) {
            return std::nullopt;
        }
        type = ApplyMatchMap(t, maybe_match_map.value());
    }

    return type;
}

}  // namespace komaru::lang
