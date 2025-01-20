#include "link.hpp"

#include <editor/kimgui/imgui_id_allocator.hpp>

namespace komaru::editor {

Link::Link(const Node& from, const Node& to, std::string label)
    : from_id_(from.GetOutPinID())
    , to_id_(to.GetInPinID())
    , source_node_id_(from.GetID())
    , target_node_id_(to.GetID())
    , label_(std::move(label)) {
    id_ = ImGui::AllocateID();
}

void Link::UpdateAndDraw(float) {
    ImNodes::Link(id_, from_id_, to_id_, label_.c_str());
}

int Link::GetID() const {
    return id_;
}

int Link::GetSourceNodeID() const {
    return source_node_id_;
}

int Link::GetTargetNodeID() const {
    return target_node_id_;
}

}
