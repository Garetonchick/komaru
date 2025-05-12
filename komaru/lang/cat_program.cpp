#include "cat_program.hpp"

#include <print>

#include <komaru/util/std_extensions.hpp>

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
    : source_pin_(source_pin),
      target_node_(target_node),
      morphism_(std::move(morphism)) {
}

const CPNode& CPOutPin::GetNode() const {
    return node_;
}

const CPOutPin::Brancher& CPOutPin::GetBrancher() const {
    return brancher_;
}

const std::deque<CPArrow>& CPOutPin::Arrows() const {
    return arrows_;
}

void CPOutPin::SetPattern(Pattern pattern) {
    brancher_ = std::move(pattern);
}

void CPOutPin::SetGuard(Guard guard) {
    brancher_ = std::move(guard);
}

CPArrow& CPOutPin::AddArrow(CPNode& target_node, MorphismPtr morphism) {
    return arrows_.emplace_back(Arrow(*this, target_node, std::move(morphism)));
}

CPOutPin::OutPin(Node& node)
    : node_(node) {
}

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

std::deque<CPOutPin>& CPNode::OutPins() {
    return out_pins_;
}

CPOutPin& CPNode::AddOutPin() {
    return out_pins_.emplace_back(CPOutPin(*this));
}

CPOutPin& CPNode::AddOutPin(Pattern pattern) {
    auto& out_pin = out_pins_.emplace_back(CPOutPin(*this));
    out_pin.SetPattern(std::move(pattern));
    return out_pin;
}

CPOutPin& CPNode::AddOutPin(Guard guard) {
    auto& out_pin = out_pins_.emplace_back(CPOutPin(*this));
    out_pin.SetGuard(std::move(guard));
    return out_pin;
}

CPNode& CPNode::SetName(std::string name) {
    name_ = std::move(name);
    return *this;
}

CPNode::Node(Type type, std::string name)
    : type_(type),
      name_(std::move(name)) {
}

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

CatProgramBuilder& CatProgramBuilder::Connect(CPOutPin& out_pin, CPNode& node,
                                              MorphismPtr morphism) {
    auto& arrow = out_pin.AddArrow(node, std::move(morphism));
    node.incoming_arrows_.push_back(&arrow);
    return *this;
}

CatProgram CatProgramBuilder::Extract() {
    return std::move(program_);
}

void DebugCatProgram(const CatProgram& program) {
    std::println("CatProgram: {}", program.GetNodes().size());

    std::unordered_map<const CPNode*, std::string> node2name;

    size_t node_index = 0;

    for (const auto& node : program.GetNodes()) {
        std::string name = node.GetName();
        if (name.empty()) {
            name = "node" + std::to_string(node_index++);
        }
        node2name[&node] = name;
    }

    for (const auto& node : program.GetNodes()) {
        std::println("Node \"{}\":", node2name[&node]);
        std::println("  Type \"{}\"", node.GetType().ToString());
        for (const auto& [i, out_pin] : util::Enumerate(node.OutPins())) {
            std::println("  OutPin {}:", i);
            std::visit(util::Overloaded{
                           [](const Pattern& pattern) {
                               std::println("    Pattern \"{}\"", pattern.ToString());
                           },
                           [](const Guard& guard) {
                               std::println("    Guard \"{}\"", guard.ToString());
                           },
                       },
                       out_pin.GetBrancher());

            std::println("    Arrows:");

            for (const auto& arrow : out_pin.Arrows()) {
                std::println("      Arrow \"{}\":", arrow.GetMorphism()->ToString());
                std::println("        Type \"{}\"", arrow.GetMorphism()->GetType().ToString());
                std::println("        Target Node \"{}\"", node2name[&arrow.TargetNode()]);
            }
        }
    }
}

}  // namespace komaru::lang
