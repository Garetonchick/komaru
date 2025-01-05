#pragma once
#include <editor/gui_element.hpp>
#include <editor/block.hpp>
#include <editor/top_down_camera.hpp>
#include <GLFW/glfw3.h>

#include <vector>

class Grid : public GuiElement {
public:
    Grid(GLFWwindow* window);

    void UpdateAndDraw(float dt) override;

private:
    void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

private:
    static constexpr float kThickness = 4.f;
    static constexpr float kStride = 20.f;

    GLFWwindow* window_; // TODO: abstract away
    TopDownCamera camera_;
    std::vector<Block> blocks_;
    // float scale_{1.f};
    // float vertical_start_{6.f};
    // float horizontal_start_{8.f};
};
