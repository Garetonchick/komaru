#include "grid.hpp"

#include <editor/gui.hpp>

#include <print>

Grid::Grid(GLFWwindow* window)
    : window_(window) {
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window_, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto self = static_cast<Grid*>(glfwGetWindowUserPointer(window));
        self->OnMouseScroll(window, xoffset, yoffset);
    });  // TODO: This should not be here

    blocks_.push_back(Block("mop 1", &camera_, {250, 100}));
    blocks_.push_back(Block("mop 2", &camera_, {450, 100}));
}

void Grid::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;

    const float scroll_speed = 0.01f;
    const float min_scale = 0.05f;
    const float max_scale = 4.f;

    float old_scale = camera_.GetScale();
    // float new_scale = old_scale + yoffset * kScrollSpeed;
    float new_scale = old_scale * (1.f + yoffset * scroll_speed);
    new_scale = std::clamp(new_scale, min_scale, max_scale);

    double xpos = 0;
    double ypos = 0;
    glfwGetCursorPos(window, &xpos, &ypos);

    ImVec2 camera_step = {(float)xpos * (1.f / old_scale - 1.f / new_scale),
                          (float)ypos * (1.f / old_scale - 1.f / new_scale)};

    camera_.SetScale(new_scale);
    camera_.Move(camera_step);
}

void Grid::UpdateAndDraw(float dt) {
    auto draw_list = ImGui::GetBackgroundDrawList();

    auto color = IM_COL32(100, 100, 100, 255);
    int win_width = 0;
    int win_height = 0;
    glfwGetWindowSize(window_, &win_width, &win_height);

    float step = kStride * camera_.GetScale();
    float thickness = kThickness * camera_.GetScale();

    ImVec2 start = camera_.GetPos();
    start.x = std::floor(start.x / kStride) * kStride;
    start.y = std::floor(start.y / kStride) * kStride;
    start -= ImVec2(1.f * kStride, 1.f * kStride);
    start = camera_.Global2Camera(start);

    for (float l = start.x; l < win_width + thickness; l += step) {
        draw_list->AddLine({l, 0}, {l, (float)win_height}, color, thickness);
    }

    for (float h = start.y; h < win_height + thickness; h += step) {
        draw_list->AddLine({0, h}, {(float)win_width, h}, color, thickness);
    }

    const ImVec2 global_test_circle_pos = {400, 400};

    ImVec2 test_circle_pos = camera_.Global2Camera(global_test_circle_pos);
    // draw_list->AddQuadFilled(
    //     test_circle_pos,
    //     test_circle_pos + ImVec2{200, 0} * camera_.GetScale(),
    //     test_circle_pos + ImVec2{200, 100} * camera_.GetScale(),
    //     test_circle_pos + ImVec2{0,100} * camera_.GetScale(),
    //     IM_COL32(255, 255, 0, 255));
    draw_list->AddCircleFilled(test_circle_pos, 30.f * camera_.GetScale(),
                               IM_COL32(255, 255, 0, 255));

    // draw_list->AddText(ImGui::GetFont(), 30, {300, 300}, IM_COL32(255, 255, 255, 255), "kek");
    // draw_list->AddText({300, 300}, IM_COL32(0, 0, 0, 255), "kek");

    for (auto& block : blocks_) {
        block.UpdateAndDraw(dt);
    }
}
