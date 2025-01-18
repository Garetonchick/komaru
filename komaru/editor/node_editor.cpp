#include "node_editor.hpp"

#include <editor/kimgui/imgui_window_guard.hpp>
#include <editor/kimgui/imgui_child_guard.hpp>
#include <imnodes/imnodes.h>

#include <iostream>

namespace komaru::editor {

NodeEditor::NodeEditor(
    std::string name, ImVec2 pos, ImVec2 size
) : name_(std::move(name)), pos_(std::move(pos)), size_(std::move(size)) {
    // Dummy nodes
    nodes_.emplace_back("Test Node 1", ImVec2{100, 100});
    nodes_.emplace_back("Test Node 2", ImVec2{500, 200});
    nodes_order_.push_back(0);
    nodes_order_.push_back(1);
}

void NodeEditor::UpdateAndDraw(float dt) {
    std::cout << "Is space key pressed: " << ImGui::IsKeyDown(ImGuiKey_Space) << std::endl;
    (void)dt;
    ImGui::Begin("node editor");
    ImNodes::BeginNodeEditor();

    ImNodes::BeginNode(0);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Test Node 1");
    ImNodes::EndNodeTitleBar();
    ImGui::Dummy(ImVec2(80.0f, 45.0f));
    ImNodes::BeginOutputAttribute(32);
    ImGui::Text("output pin 1");
    ImNodes::EndOutputAttribute();

    ImNodes::BeginOutputAttribute(42);
    ImGui::Text("output pin 2");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    ImNodes::BeginNode(1);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Test Node 2");
    ImNodes::EndNodeTitleBar();
    ImGui::Dummy(ImVec2(80.0f, 45.0f));

    ImNodes::BeginInputAttribute(44);
    ImGui::Text("input pin");
    ImNodes::EndInputAttribute();

    ImNodes::EndNode();

    ImNodes::Link(89, 42, 44);

    ImNodes::MiniMap();
    ImNodes::EndNodeEditor();
    ImGui::End();

    // if(ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    //     ImNodes::EditorContextMoveToNode(1);
    // }

    // ImGui::SetNextWindowPos(pos_, ImGuiCond_Once);
    // ImGui::SetNextWindowSize(size_, ImGuiCond_Once);

    // ImGui::WindowGuard whole_guard(name_.c_str());
    // if(whole_guard.Skip()) {
    //     return;
    // }

    // ImGui::ChildGuard child_guard("editor_child", ImVec2(0, 0), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    // if(nodes_.empty()) {
    //     return;
    // }

    // for(size_t idx : nodes_order_) {
    //     nodes_[idx].UpdateAndDraw(dt);
    // }

    // for(int64_t oidx = nodes_order_.size() - 1; oidx >= 0; --oidx) {
    //     size_t idx = nodes_order_[oidx];
    //     if(nodes_[idx].IsMaybeSelected()) {
    //         nodes_order_.erase(nodes_order_.begin() + oidx);
    //         nodes_order_.push_back(idx);
    //         break;
    //     }
    // }
}

}
