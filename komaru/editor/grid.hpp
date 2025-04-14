#pragma once
#include <komaru/editor/gui_element.hpp>
#include <komaru/editor/block.hpp>
#include <komaru/editor/top_down_camera.hpp>
#include <GLFW/glfw3.h>

#include <vector>

class Grid : public GuiElement {
public:
    explicit Grid(GLFWwindow* window);

    void UpdateAndDraw(float dt) override;

private:
    void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

private:
    static constexpr float kThickness = 4.f;
    static constexpr float kStride = 20.f;

    GLFWwindow* window_;  // TODO: abstract away
    TopDownCamera camera_;
    std::vector<Block> blocks_;
};
