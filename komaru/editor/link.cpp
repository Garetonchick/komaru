#include "link.hpp"

#include <editor/kimgui/imgui_id_allocator.hpp>

namespace komaru::editor {

Link::Link(const Node& from, const Node& to, std::string label)
    : from_id_(from.GetOutPinID())
    , to_id_(to.GetInPinID())
    , label_(std::move(label)) {
    id_ = ImGui::AllocateID();
}

void Link::UpdateAndDraw(float) {
    ImNodes::Link(id_, from_id_, to_id_, label_.c_str());
    // ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // constexpr ImU32 kArrowColor = IM_COL32(255, 0, 255, 255);

    // draw_list->AddLine({0, 0}, {100, 100}, kArrowColor);
}

int Link::GetID() const {
    return id_;
}

}
