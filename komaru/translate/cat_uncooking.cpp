#include "cat_uncooking.hpp"

#include <komaru/util/std_extensions.hpp>

namespace komaru::translate {

using RawNodeID = RawCatProgram::NodeID;
using CPNode = lang::CatProgram::Node;
using CPOutPin = lang::CatProgram::OutPin;
using CPArrow = lang::CatProgram::Arrow;
using CPBrancher = CPOutPin::Brancher;

RawNodeID UncookNode(RawCatProgram& raw_program, const CPNode& node);
void UncookArrow(RawCatProgram& raw_program, const CPArrow& arrow, RawNodeID source_id,
                 RawNodeID target_id, size_t pin_idx);
std::string ToString(const CPBrancher& brancher);

RawCatProgram Uncook(const lang::CatProgram& program) {
    auto& nodes = program.GetNodes();
    RawCatProgram raw_program;
    std::unordered_map<const CPNode*, RawNodeID> node2id;

    for (const auto& node : nodes) {
        node2id[&node] = UncookNode(raw_program, node);
    }

    for (const auto& node : nodes) {
        for (const auto [pin_idx, out_pin] : util::Enumerate(node.OutPins())) {
            for (const auto& arrow : out_pin.Arrows()) {
                UncookArrow(raw_program, arrow, node2id[&node], node2id[&arrow.TargetNode()],
                            pin_idx);
            }
        }
    }

    return raw_program;
}

RawNodeID UncookNode(RawCatProgram& raw_program, const CPNode& node) {
    std::optional<std::string> maybe_name;
    std::vector<std::string> branchers;

    if (!node.GetName().empty()) {
        maybe_name.emplace(node.GetName());
    }

    auto& out_pins = node.OutPins();
    branchers.reserve(out_pins.size());

    for (const auto& pin : out_pins) {
        branchers.emplace_back(ToString(pin.GetBrancher()));
    }

    return raw_program.NewNodeGeneric(std::string(node.GetType().GetName()), maybe_name, branchers,
                                      !node.IncomingArrows().empty());
}

void UncookArrow(RawCatProgram& raw_program, const CPArrow& arrow, RawNodeID source_id,
                 RawNodeID target_id, size_t pin_idx) {
    std::string morphism_str = arrow.GetMorphism()->ToString();

    if (arrow.SourcePin().GetNode().GetType() != lang::Type::Singleton() &&
        arrow.GetMorphism()->GetSource() == lang::Type::Singleton()) {
        morphism_str = "! " + morphism_str;
    }

    raw_program.Connect(source_id, target_id, pin_idx, morphism_str);
}

std::string ToString(const CPBrancher& brancher) {
    return std::visit(
        [](const auto& b) {
            return b.ToString();
        },
        brancher);
}

}  // namespace komaru::translate
