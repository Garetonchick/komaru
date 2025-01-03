#include "block.hpp"

#include <editor/imgui_window_guard.hpp>

#include <print>

Block::Block(std::string name, ImVec2 pos, ImVec2 size, ImVec2 pivot)
    : name_(std::move(name)), pos_(pos), pivot_(pivot), size_(size) {

}

void Block::UpdateAndDraw(float dt) {
    BeforeWindow();

    {
        ImGuiWindowFlags flags = 0;
        if(disable_resize_) {
            flags |= ImGuiWindowFlags_NoResize;
        }

        ImGuiWindowGuard window_guard(name_.c_str(), nullptr, flags);

        Update(dt);
        Draw();
    }
}

void Block::BeforeWindow() {
    ImGui::SetNextWindowSize(size_, ImGuiCond_Once);
    ImGui::SetNextWindowPos(pos_, ImGuiCond_Once, pivot_);
}

void Block::Update(float dt) {
    (void)dt;
    size_ = ImGui::GetWindowSize();
}

float Length(ImVec2 vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

void Block::Draw() {
    auto io = ImGui::GetIO();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_padding = ImGui::GetStyle().WindowPadding;
    ImVec2 content_pos = window_pos + window_padding;
    const float kCircleRadius = 10.f;
    ImVec2 circle_center = {content_pos.x + size_.x - kCircleRadius * 0.5f, content_pos.y + size_.y * 0.5f};

    ImU32 color = IM_COL32(255, 255, 255, 255);

    if(Length(io.MousePos - circle_center) < kCircleRadius) {
        color = IM_COL32(0, 255, 0, 255);
        disable_resize_ = true;
    } else {
        disable_resize_ = false;
    }

    draw_list->PushClipRectFullScreen();
    draw_list->AddCircleFilled(
        circle_center,
        kCircleRadius,
        color,
        36
    );


    draw_list->PopClipRect();
}
