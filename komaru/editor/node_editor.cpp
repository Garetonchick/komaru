#include "node_editor.hpp"

#include <editor/kimgui/imgui_window_guard.hpp>
#include <editor/kimgui/imgui_child_guard.hpp>

namespace komaru::editor {

NodeEditor::NodeEditor(
    std::string name, ImVec2 pos, ImVec2 size
) : name_(std::move(name)), pos_(std::move(pos)), size_(std::move(size)) {
    // Dummy nodes
    nodes_.emplace_back("Test Node 1", ImVec2{100, 100});
    nodes_.emplace_back("Test Node 2", ImVec2{500, 200});
}

void NodeEditor::UpdateAndDraw(float dt) {
    ImGui::SetNextWindowPos(pos_, ImGuiCond_Once);
    ImGui::SetNextWindowSize(size_, ImGuiCond_Once);

    ImGui::WindowGuard whole_guard(name_.c_str());
    if(whole_guard.Skip()) {
        return;
    }

    ImGui::ChildGuard child_guard("editor_child", ImVec2(0, 0), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    for(auto& node : nodes_) {
        node.UpdateAndDraw(dt);
    }
}

}
