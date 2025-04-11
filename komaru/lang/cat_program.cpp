#include "cat_program.hpp"

namespace komaru::lang {

using CPNode = CatProgram::Node;
using CPOutPin = CatProgram::OutPin;
using CPArrow = CatProgram::Arrow;

const CPNode& CPArrow::TargetNode() const {
    return target_node_;
}

const CPOutPin& CPArrow::SourcePin() const {
    return source_pin_;
}

const MorphismPtr& CPArrow::GetMorphism() const {
    return morphism_;
}

CPArrow::Arrow(CPOutPin& source_pin, CPNode& target_node, MorphismPtr morphism)
    : source_pin_(source_pin)
    , target_node_(target_node)
    , morphism_(std::move(morphism)) {
}

const CPNode& CPOutPin::GetNode() const {
    return node_;
}

const Pattern& CPOutPin::GetPattern() const {
    return pattern_;
}

const std::deque<CPArrow>& CPOutPin::Arrows() const {
    return arrows_;
}

void CPOutPin::SetPattern(Pattern pattern) {
    pattern_ = std::move(pattern);
}

CPArrow& CPOutPin::AddArrow(CPNode& target_node, MorphismPtr morphism) {
    return arrows_.emplace_back(Arrow(*this, target_node, std::move(morphism)));
}

CPOutPin::OutPin(Node& node) : node_(node) {}

Type CPNode::GetType() const {
    return type_;
}

const std::vector<CPArrow*>& CPNode::IncomingArrows() const {
    return incoming_arrows_;
}

const std::deque<CPOutPin>& CPNode::OutPins() const {
    return out_pins_;
}

const std::string& CPNode::GetName() const {
    return name_;
}

CPOutPin& CPNode::AddOutPin() {
    return out_pins_.emplace_back(CPOutPin(*this));
}

CPOutPin& CPNode::AddOutPin(Pattern pattern) {
    auto& out_pin = out_pins_.emplace_back(CPOutPin(*this));
    out_pin.SetPattern(std::move(pattern));
    return out_pin;
}

CPNode& CPNode::SetName(std::string name) {
    name_ = std::move(name);
    return *this;
}

CPNode::Node(Type type, std::string name)
    : type_(type), name_(std::move(name)) {}

const std::deque<CPNode>& CatProgram::GetNodes() const {
    return nodes_;
}

CPNode& CatProgramBuilder::NewNode(Type type, std::string name) {
    return program_.nodes_.emplace_back(CPNode(type, std::move(name)));
}

std::pair<CPNode&, CPOutPin&> CatProgramBuilder::NewNodeWithPin(Type type, std::string name) {
    auto& node = NewNode(type, std::move(name));
    auto& pin = node.AddOutPin();
    return std::tie(node, pin);
}

CatProgramBuilder& CatProgramBuilder::Connect(CPOutPin& out_pin, CPNode& node, MorphismPtr morphism) {
    auto& arrow = out_pin.AddArrow(node, std::move(morphism));
    node.incoming_arrows_.push_back(&arrow);
    return *this;
}

CatProgram CatProgramBuilder::Extract() {
    return std::move(program_);
}

}
