#pragma once
#include <vector>
#include <string>
#include <optional>

namespace komaru::translate {

class RawCatProgram {
public:
    using NodeID = size_t;

    struct Node {
        std::string type;
        std::optional<std::string> name;
        std::vector<std::string> branchers;
        bool has_input;
    };

    struct Connection {
        NodeID source_node;
        NodeID target_node;
        size_t pin_idx;
        std::string morphism;
    };

public:
    RawCatProgram();

    NodeID NewNodeGeneric(const std::string& type, std::optional<std::string> name,
                          const std::vector<std::string>& branchers, bool has_input);

    // Nodes without input pins
    NodeID NewRootNode(const std::string& type);
    NodeID NewRootNodeWithName(const std::string& type, std::optional<std::string> name);
    NodeID NewRootNode(const std::string& type, const std::vector<std::string>& branchers);
    NodeID NewRootNodeWithName(const std::string& type, std::optional<std::string> name,
                               const std::vector<std::string>& branchers);

    // Nodes with one input pin
    NodeID NewNode(const std::string& type);
    NodeID NewNodeWithName(const std::string& type, std::optional<std::string> name);
    NodeID NewNode(const std::string& type, const std::vector<std::string>& branchers);
    NodeID NewNodeWithName(const std::string& type, std::optional<std::string> name,
                           const std::vector<std::string>& branchers);

    // Nodes with one input pin and no end pin
    NodeID NewEndNode(const std::string& type);

    RawCatProgram& Connect(NodeID source_node, NodeID target_node, size_t pin_idx,
                           const std::string& morphism);

    const std::vector<Node>& GetNodes() const;
    const std::vector<Connection>& GetConnections() const;

private:
    std::vector<Node> nodes_;
    std::vector<Connection> connections_;
};

}  // namespace komaru::translate
