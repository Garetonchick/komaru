#include "node.hpp"

#include <editor/kimgui/imgui_id_allocator.hpp>
#include <editor/kimgui/imgui_id_guard.hpp>

namespace komaru::editor {

Node::Node(std::string title, ImVec2 pos)
    : title_(title)
    , pos_(pos)
    , size_(100, 100) {
    id_ = ImGui::AllocateID();
    (void)id_;
}

void Node::UpdateAndDraw(float) {
    ImGui::IdGuard guard(id_);

    ImVec2 offset = ImGui::GetWindowPos();
    ImVec2 ul = offset + pos_;
    ImU32 node_color = kNodeColor;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();

    draw_list->ChannelsSplit(2);

    draw_list->ChannelsSetCurrent(1);
    ImGui::SetCursorScreenPos(ul + ImVec2{10.f, 0.f});
    ImGui::Text("%s", title_.c_str());
    ImGui::PushItemWidth(60.0f);
    ImGui::SetCursorScreenPos(ImVec2{ul.x + 10.f, ImGui::GetCursorScreenPos().y});
    ImGui::SliderFloat("kek", &v_, 0.f, 1.f);
    ImGui::PopItemWidth();
    ImGui::SetCursorScreenPos(ImVec2{ul.x + 10.f, ImGui::GetCursorScreenPos().y});
    ImGui::SmallButton("lol");


    draw_list->ChannelsSetCurrent(0);
    ImGui::SetCursorScreenPos(ul);
    ImGui::InvisibleButton("body", size_);

    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        pos_ += io.MouseDelta;
    }

    if(ImGui::IsItemHovered()) {
        node_color = IM_COL32(0, 255, 0, 255);
    }

    draw_list->AddQuadFilled(
        ul,
        ul + ImVec2{size_.x, 0.f},
        ul + size_,
        ul + ImVec2{0.f, size_.y},
        node_color
    );

    draw_list->ChannelsMerge();
}

}
