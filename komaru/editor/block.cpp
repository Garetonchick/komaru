#include "block.hpp"

#include <editor/imgui_window_guard.hpp>
#include <editor/gui_math.hpp>

#include <print>

Block::Block(std::string name, const TopDownCamera* camera, ImVec2 pos, ImVec2 size, ImVec2 pivot)
    : name_(std::move(name))
    , global_pos_(pos)
    , global_size_(size)
    , initial_pivot_(pivot)
    , local_size_(size)
    , local_pos_(pos)
    , camera_(camera) {
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
    ImGui::SetNextWindowSize(global_size_, ImGuiCond_Once);
    ImGui::SetNextWindowPos(global_pos_, ImGuiCond_Once, initial_pivot_);
}

void Block::Update(float dt) {
    (void)dt;

    static bool was_smth = false;

    auto local_step = ImGui::GetWindowPos() - local_pos_;
    auto size_mul = ImGui::GetWindowSize() / local_size_;
    if(!IsNearZero(local_step) && !was_smth) {
        global_pos_ += local_step / camera_->GetScale();
    }
    if(Length(size_mul - ImVec2{1.f, 1.f}) > 1e-2 && !was_smth) {
        global_size_ *= size_mul;
    }

    local_pos_ = camera_->Global2Camera(global_pos_);
    local_size_ = global_size_ * camera_->GetScale();

    if(!VecEq(local_pos_, ImGui::GetWindowPos()) || !VecEq(local_size_, ImGui::GetWindowSize())) {
        was_smth = true;
    } else {
        was_smth = false;
    }

    ImGui::SetWindowPos(local_pos_, ImGuiCond_Always);
    ImGui::SetWindowSize(local_size_);
}

void Block::Draw() {
    auto io = ImGui::GetIO();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const float kBaseCircleRadius = 10.f;
    float circle_r = kBaseCircleRadius * camera_->GetScale();
    ImVec2 circle_center = {
        local_pos_.x + local_size_.x,
        local_pos_.y + local_size_.y * 0.5f
    };

    ImU32 color = IM_COL32(255, 255, 255, 255);

    // Move to update
    if(Length(io.MousePos - circle_center) < circle_r) {
        color = IM_COL32(0, 255, 0, 255);
        disable_resize_ = true;
    } else {
        disable_resize_ = false;
    }

    draw_list->PushClipRectFullScreen();
    draw_list->AddCircleFilled(
        circle_center,
        circle_r,
        color,
        36
    );


    draw_list->PopClipRect();
}
