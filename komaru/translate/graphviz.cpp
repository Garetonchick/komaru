#include "graphviz.hpp"

#include <komaru/util/std_extensions.hpp>
#include <komaru/util/string.hpp>
#include <komaru/translate/cat_uncooking.hpp>

#include <format>

namespace komaru::translate {

using RNode = RawCatProgram::Node;
using RNodeID = RawCatProgram::NodeID;
using RConnection = RawCatProgram::Connection;

struct Edge {
    RNodeID node_id;
    size_t conn_idx;
};

struct Component {
    std::vector<RNodeID> nodes;
    std::vector<size_t> connections;
};

using Graph = std::vector<std::vector<Edge>>;

bool IsRegularNode(const RNode& node);
std::string MakeNodeLabel(const RNode& node);
std::string MakeNodeRef(const RNode& node, RNodeID id);
std::string MakePinRef(const RNode& node, RNodeID id, size_t pin_idx);
std::string EscapeGrpaphvizStr(const std::string& s);

std::vector<Component> GetGraphComponents(const Graph& graph) {
    std::vector<char> visited(graph.size(), false);

    auto dfs = [&](auto& self, Component& comp, size_t v) -> void {
        visited[v] = true;
        comp.nodes.push_back(v);

        for (const Edge& e : graph[v]) {
            comp.connections.push_back(e.conn_idx);
            if (!visited[e.node_id]) {
                self(self, comp, e.node_id);
            }
        }
    };

    std::vector<size_t> deg(graph.size());

    for (RNodeID id = 0; id < graph.size(); ++id) {
        for (const Edge& e : graph[id]) {
            ++deg[e.node_id];
        }
    }

    std::vector<Component> components;

    for (RNodeID id = 0; id < graph.size(); ++id) {
        if (!deg[id]) {
            components.emplace_back(Component{});
            dfs(dfs, components.back(), id);
        }
    }

    return components;
}

Graph BuildGraph(const RawCatProgram& program) {
    auto& nodes = program.GetNodes();
    auto& connections = program.GetConnections();

    Graph graph(nodes.size());

    for (const auto& [i, conn] : util::Enumerate(connections)) {
        graph[conn.source_node].emplace_back(Edge{.node_id = conn.target_node, .conn_idx = i});
    }

    return graph;
}

std::string ComponentToGraphviz(const Component& component, const std::vector<RNode>& nodes,
                                const std::vector<RConnection>& connections) {
    RNodeID root_id = component.nodes[0];
    std::string comp_name = nodes[root_id].name.value();
    std::vector<RNodeID> tag_nodes;

    for (RNodeID node_id : component.nodes) {
        auto& node = nodes[node_id];
        if (node_id != root_id && node.name.has_value()) {
            tag_nodes.push_back(node_id);
        }
    }

    std::string gstr = "";

    // name nodes
    gstr += std::format(
        "name_node{0} [label=\"{1}\", shape=circle, margin=0];\n"
        "name_sup_node{0} [label=\"\", shape=plaintext, margin=0];\n",
        root_id, EscapeGrpaphvizStr(comp_name));

    // tag nodes
    for (RNodeID node_id : tag_nodes) {
        auto& node = nodes[node_id];
        gstr += std::format("tag_node{} [label=\"{}\", shape=plaintext, margin=0];\n", node_id,
                            EscapeGrpaphvizStr(node.name.value()));
    }

    gstr += "\n";

    // regular nodes
    for (RNodeID node_id : component.nodes) {
        auto& node = nodes[node_id];
        std::string label = MakeNodeLabel(node);

        gstr += std::format("node{} [label=\"{}\", shape=record];\n", node_id, label);
    }

    // name node's connections
    gstr += "\n";
    gstr += std::format(
        "name_node{0} -> name_sup_node{0} [style=invisible, arrowhead=none];\n"
        "name_sup_node{0} -> node{0} [style=invisible, arrowhead=none];\n",
        root_id);

    for (RNodeID node_id : tag_nodes) {
        gstr += std::format(
            "tag_node{0} -> node{0} [style=invisible, arrowhead=none, minlen=0.5];\n", node_id);
    }

    gstr += "\n";

    // regular node's connections
    for (size_t conn_idx : component.connections) {
        auto& conn = connections[conn_idx];
        std::string source = MakePinRef(nodes[conn.source_node], conn.source_node, conn.pin_idx);
        std::string target = MakeNodeRef(nodes[conn.target_node], conn.target_node);

        gstr += std::format("{} -> {} [label=\"{}\"];\n", source, target,
                            EscapeGrpaphvizStr(conn.morphism));
    }

    // ranks for name nodes and tag nodes
    gstr += "\n";
    gstr += std::format("{{ rank=same; name_node{0}; name_sup_node{0}; }}\n", root_id);

    for (RNodeID node_id : tag_nodes) {
        gstr += std::format("{{ rank=same; tag_node{0}; node{0}; }};\n", node_id);
    }

    return gstr;
}

std::string ToGraphviz(const RawCatProgram& program) {
    auto& nodes = program.GetNodes();
    auto& connections = program.GetConnections();

    auto graph = BuildGraph(program);
    auto components = GetGraphComponents(graph);

    std::string gstr =
        "digraph G {\n"
        "  rankdir=LR;\n"
        "  nodesep=0.3;\n"
        "  node [fontsize=12]\n\n";

    for (const auto& [i, component] : util::Enumerate(components)) {
        gstr += std::format("  subgraph g{} {{\n", i);
        gstr += util::Indent(ComponentToGraphviz(component, nodes, connections), "    ");

        gstr += "  }\n";
        if (i + 1 != components.size()) {
            gstr += "\n";
        }
    }

    gstr += "}";

    return gstr;
}

std::string ToGraphviz(const lang::CatProgram& program) {
    return ToGraphviz(Uncook(program));
}

bool IsRegularNode(const RNode& node) {
    return node.branchers.empty() || (node.branchers.size() == 1 && node.branchers[0] == "*");
}

std::string MakeNodeLabel(const RNode& node) {
    std::string etype = EscapeGrpaphvizStr(node.type);
    if (IsRegularNode(node)) {
        return etype;
    }

    std::string label = std::format("{{<main>{}|{{", etype);

    for (size_t i = 0; i < node.branchers.size(); ++i) {
        label += std::format("<out{}>{}", i, EscapeGrpaphvizStr(node.branchers[i]));
        if (i + 1 != node.branchers.size()) {
            label += "|";
        }
    }

    label += "}}";

    return label;
}

std::string MakeNodeRef(const RNode& node, RNodeID id) {
    if (IsRegularNode(node)) {
        return std::format("node{}", id);
    }
    return std::format("node{}:main", id);
}

std::string MakePinRef(const RNode& node, RNodeID id, size_t pin_idx) {
    if (IsRegularNode(node)) {
        return std::format("node{}", id);
    }

    return std::format("node{}:out{}", id, pin_idx);
}

std::string EscapeGrpaphvizStr(const std::string& s) {
    return util::Escape(s, "|{}<>");
}

}  // namespace komaru::translate
