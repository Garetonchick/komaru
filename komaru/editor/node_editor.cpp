#include "node_editor.hpp"

#include <editor/kimgui/imgui_window_guard.hpp>
#include <editor/kimgui/imgui_child_guard.hpp>

#include <algorithm>
#include <print>

namespace komaru::editor {

NodeEditor::NodeEditor(std::string name, ImVec2 pos, ImVec2 size)
    : name_(std::move(name)),
      pos_(std::move(pos)),
      size_(std::move(size)) {
    // Dummy nodes
    nodes_.emplace_back("START", ImVec2{100, 300}, NodeType::Start);
    nodes_.emplace_back("INT x INT", ImVec2{250, 400});
    nodes_.emplace_back("INT", ImVec2{500, 400});
    nodes_.emplace_back("TERMINAL", ImVec2{650, 300}, NodeType::Terminal);
    links_.emplace_back(nodes_[0], nodes_[1]);
    links_.emplace_back(nodes_[1], nodes_[2], "+");
    links_.emplace_back(nodes_[2], nodes_[3]);
}

void NodeEditor::UpdateAndDraw(float dt) {
    ImNodes::PushStyleVar(ImNodesStyleVar_NodeCornerRounding, 50.f);
    ImNodes::PushStyleVar(ImNodesStyleVar_LinkLabelTextScale, 4.f);
    ImNodes::GetIO().LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

    ImGui::SetNextWindowPos(pos_);
    ImGui::SetNextWindowSize(size_, ImGuiCond_Once);

    ImGui::Begin("node editor");
    ImNodes::BeginNodeEditor();

    for (auto& node : nodes_) {
        node.UpdateAndDraw(dt);
    }

    for (auto& link : links_) {
        link.UpdateAndDraw(dt);
    }

    ImNodes::MiniMap();

    ImNodes::EndNodeEditor();

    // if( ImNodes::IsEditorHovered() && ImGui::GetIO().MouseWheel != 0 )
    // {
    //     float zoom = ImNodes::EditorContextGetZoom() + ImGui::GetIO().MouseWheel * 0.1f;
    //     ImNodes::EditorContextSetZoom( zoom, ImGui::GetMousePos() );
    // }

    int out_id, in_id;
    if (ImNodes::IsLinkCreated(&out_id, &in_id)) {
        Node* source_node = GetNodeByOutID(out_id);
        Node* target_node = GetNodeByInID(in_id);

        assert(source_node != nullptr && target_node != nullptr);
        bool bad_link = false;

        for (auto& link : links_) {
            if (link.GetSourceNodeID() == source_node->GetID() ||
                link.GetTargetNodeID() == target_node->GetID()) {
                bad_link = true;
                break;
            }
        }

        if (!bad_link) {
            links_.push_back(Link(*source_node, *target_node));
            std::println("Created new link. Total links {}", links_.size());
        }
    }

    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id)) {
        assert(DestroyLinkByID(link_id));
        std::println("Destroyed link. Total links {}", links_.size());
    }

    ImGui::End();

    ImNodes::PopStyleVar(2);
}

Node* NodeEditor::GetNodeByID(int id) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(), [id](const Node& node) {
        return node.GetID() == id;
    });
    return it == nodes_.end() ? nullptr : &*it;
}
Node* NodeEditor::GetNodeByInID(int in_id) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(), [in_id](const Node& node) {
        return node.GetInPinID() == in_id;
    });
    return it == nodes_.end() ? nullptr : &*it;
}
Node* NodeEditor::GetNodeByOutID(int out_id) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(), [out_id](const Node& node) {
        return node.GetOutPinID() == out_id;
    });
    return it == nodes_.end() ? nullptr : &*it;
}

bool NodeEditor::DestroyLinkByID(int link_id) {
    size_t del_cnt = std::erase_if(links_, [link_id](const Link& link) {
        return link.GetID() == link_id;
    });
    assert(del_cnt <= 1);
    return del_cnt == 1;
}

}  // namespace komaru::editor
