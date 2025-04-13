#include "block.hpp"

#include <editor/kimgui/imgui_window_guard.hpp>
#include <editor/gui_math.hpp>

#include <print>

Block::Block(std::string name, const TopDownCamera* camera, ImVec2 pos, ImVec2 size, ImVec2 pivot)
    : name_(std::move(name)),
      global_pos_(pos),
      global_size_(size),
      initial_pivot_(pivot),
      camera_(camera),
      window_manager_(pos, size) {
}

void Block::UpdateAndDraw(float dt) {
    BeforeWindow();

    {
        ImGuiWindowFlags flags = 0;
        if (disable_resize_) {
            flags |= ImGuiWindowFlags_NoResize;
        }

        ImGui::WindowGuard window_guard(name_.c_str(), nullptr, flags);

        Update(dt);
        Draw();
    }
}

void Block::BeforeWindow() {
    ImGui::SetNextWindowSize(global_size_, ImGuiCond_Once);
    ImGui::SetNextWindowPos(global_pos_, ImGuiCond_Once, initial_pivot_);
}

void Block::Update(float dt) {
    (void)dt;

    auto pos_move = window_manager_.PropogateMove();
    auto size_move = window_manager_.PropogateResize();
    global_pos_ += pos_move / camera_->GetScale();
    global_size_ += size_move / camera_->GetScale();

    auto new_pos = camera_->Global2Camera(global_pos_);
    auto new_size = global_size_ * camera_->GetScale();
    window_manager_.SetPos(new_pos);
    window_manager_.SetSize(new_size);
}

void Block::Draw() {
    auto io = ImGui::GetIO();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const float base_circle_radius = 10.f;
    float circle_r = base_circle_radius * camera_->GetScale();
    ImVec2 local_pos = window_manager_.GetPos();
    ImVec2 local_size = window_manager_.GetSize();
    ImVec2 circle_center = {local_pos.x + local_size.x, local_pos.y + local_size.y * 0.5f};

    ImU32 color = IM_COL32(255, 255, 255, 255);

    // Move to update
    if (Length(io.MousePos - circle_center) < circle_r) {
        color = IM_COL32(0, 255, 0, 255);
        disable_resize_ = true;
    } else {
        disable_resize_ = false;
    }

    draw_list->PushClipRectFullScreen();
    draw_list->AddCircleFilled(circle_center, circle_r, color, 36);

    draw_list->PopClipRect();
}
