#include "node_editor.hpp"

#include <editor/kimgui/imgui_window_guard.hpp>
#include <editor/kimgui/imgui_child_guard.hpp>

#include <iostream>

namespace komaru::editor {

NodeEditor::NodeEditor(
    std::string name, ImVec2 pos, ImVec2 size
) : name_(std::move(name)), pos_(std::move(pos)), size_(std::move(size)) {
    // Dummy nodes
    nodes_.emplace_back("int", ImVec2{100, 100});
    nodes_.emplace_back("int", ImVec2{500, 200});
    nodes_.emplace_back("int x int", ImVec2{300, 100});
}

void NodeEditor::UpdateAndDraw(float dt) {
    ImGui::SetNextWindowPos(pos_);
    ImGui::SetNextWindowSize(size_, ImGuiCond_Once);

    ImGui::Begin("node editor");
    ImNodes::BeginNodeEditor();

    for(auto& node : nodes_) {
        node.UpdateAndDraw(dt);
    }

    ImNodes::MiniMap();

    ImNodes::EndNodeEditor();
    ImGui::End();
}

}
