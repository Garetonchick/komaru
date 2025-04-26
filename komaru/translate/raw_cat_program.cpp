#include "raw_cat_program.hpp"

#include <komaru/util/string.hpp>

namespace komaru::translate {

using RNodeID = RawCatProgram::NodeID;
using RNode = RawCatProgram::Node;
using RConnection = RawCatProgram::Connection;

RawCatProgram::RawCatProgram() {
}

RNodeID RawCatProgram::NewNodeGeneric(const std::string& type, std::optional<std::string> name,
                                      const std::vector<std::string>& branchers, bool has_input) {
    nodes_.emplace_back(RNode{.type = util::Strip(type),
                              .name = name.transform(util::Strip),
                              .branchers = {},
                              .has_input = has_input});

    nodes_.back().branchers.reserve(branchers.size());

    for (const auto& s : branchers) {
        nodes_.back().branchers.emplace_back(util::Strip(s));
    }

    return nodes_.size() - 1;
}

RNodeID RawCatProgram::NewRootNode(const std::string& type) {
    return NewNodeGeneric(type, std::nullopt, {"*"}, false);
}

RNodeID RawCatProgram::NewRootNode(const std::string& type,
                                   const std::vector<std::string>& branchers) {
    return NewNodeGeneric(type, std::nullopt, branchers, false);
}

RNodeID RawCatProgram::NewRootNodeWithName(const std::string& type,
                                           std::optional<std::string> name) {
    return NewNodeGeneric(type, std::move(name), {"*"}, false);
}

RNodeID RawCatProgram::NewRootNodeWithName(const std::string& type, std::optional<std::string> name,
                                           const std::vector<std::string>& branchers) {
    return NewNodeGeneric(type, std::move(name), branchers, false);
}

RNodeID RawCatProgram::NewNode(const std::string& type) {
    return NewNodeGeneric(type, std::nullopt, {"*"}, true);
}

RNodeID RawCatProgram::NewNode(const std::string& type, const std::vector<std::string>& branchers) {
    return NewNodeGeneric(type, std::nullopt, branchers, true);
}

RNodeID RawCatProgram::NewNodeWithName(const std::string& type, std::optional<std::string> name) {
    return NewNodeGeneric(type, std::move(name), {"*"}, true);
}

RNodeID RawCatProgram::NewNodeWithName(const std::string& type, std::optional<std::string> name,
                                       const std::vector<std::string>& branchers) {
    return NewNodeGeneric(type, std::move(name), branchers, true);
}

RNodeID RawCatProgram::NewEndNode(const std::string& type) {
    return NewNodeGeneric(type, std::nullopt, {}, true);
}

RawCatProgram& RawCatProgram::Connect(RNodeID source_node, RNodeID target_node, size_t pin_idx,
                                      const std::string& morphism) {
    if (!nodes_[target_node].has_input) {
        throw std::runtime_error("node without input pin can't have an incoming connection");
    }

    if (pin_idx >= nodes_[source_node].branchers.size()) {
        throw std::runtime_error("invalid index for out pin");
    }

    connections_.emplace_back(Connection{.source_node = source_node,
                                         .target_node = target_node,
                                         .pin_idx = pin_idx,
                                         .morphism = util::Strip(morphism)});

    return *this;
}

const std::vector<RNode>& RawCatProgram::GetNodes() const {
    return nodes_;
}

const std::vector<RConnection>& RawCatProgram::GetConnections() const {
    return connections_;
}

}  // namespace komaru::translate
