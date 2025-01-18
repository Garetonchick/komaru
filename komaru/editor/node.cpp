#include "node.hpp"

#include <editor/kimgui/imgui_id_allocator.hpp>

namespace komaru::editor {

Node::Node(std::string type, ImVec2 pos)
    : type_(std::move(type))
    , pos_(pos)
    , size_(100, 100) {
    id_ = ImGui::AllocateID();
    in_id_ = ImGui::AllocateID();
    out_id_ = ImGui::AllocateID();
    ImNodes::SetNodeGridSpacePos(id_, pos_);
}

void Node::UpdateAndDraw(float) {
    ImNodes::BeginNode(id_);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(type_.c_str());
    ImNodes::EndNodeTitleBar();

    ImGui::Dummy(size_);

    ImNodes::BeginInputAttribute(in_id_);
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(out_id_);
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}

}
