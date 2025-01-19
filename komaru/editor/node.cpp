#include "node.hpp"

#include <editor/kimgui/imgui_id_allocator.hpp>

namespace komaru::editor {

Node::Node(std::string label, ImVec2 pos, NodeType type)
    : label_(std::move(label))
    , pos_(pos)
    , size_(100, 100)
    , type_(type) {
    id_ = ImGui::AllocateID();
    in_id_ = ImGui::AllocateID();
    out_id_ = ImGui::AllocateID();
    ImNodes::SetNodeGridSpacePos(id_, pos_);
}

void Node::UpdateAndDraw(float) {
    ImNodes::BeginNode(id_);

    // ImNodes::BeginNodeTitleBar();
    // ImNodes::EndNodeTitleBar();

    ImGui::SetWindowFontScale(2.f);
    ImGui::TextUnformatted(label_.c_str());
    ImGui::SetWindowFontScale(1.f);

    // ImGui::Dummy(size_);


    if(type_ != NodeType::Start) {
        ImNodes::BeginInputAttribute(in_id_);
        ImNodes::EndInputAttribute();
    }

    if(type_ != NodeType::Terminal) {
        ImNodes::BeginOutputAttribute(out_id_);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

int Node::GetInPinID() const {
    return in_id_;
}

int Node::GetOutPinID() const {
    return out_id_;
}

}
