#include "node_editor.hpp"

#include <editor/kimgui/imgui_window_guard.hpp>
#include <editor/kimgui/imgui_child_guard.hpp>

namespace komaru::editor {

NodeEditor::NodeEditor(
    std::string name, ImVec2 pos, ImVec2 size
) : name_(std::move(name)), pos_(std::move(pos)), size_(std::move(size)) {
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

    ImGui::SetNextWindowPos(pos_);
    ImGui::SetNextWindowSize(size_, ImGuiCond_Once);

    ImGui::Begin("node editor");
    ImNodes::BeginNodeEditor();

    for(auto& node : nodes_) {
        node.UpdateAndDraw(dt);
    }

    for(auto& link : links_) {
        link.UpdateAndDraw(dt);
    }

    ImNodes::MiniMap();

    ImNodes::EndNodeEditor();

    if( ImNodes::IsEditorHovered() && ImGui::GetIO().MouseWheel != 0 )
    {
        float zoom = ImNodes::EditorContextGetZoom() + ImGui::GetIO().MouseWheel * 0.1f;
        ImNodes::EditorContextSetZoom( zoom, ImGui::GetMousePos() );
    }

    ImGui::End();

    ImNodes::PopStyleVar(2);
}

}
