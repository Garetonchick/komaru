#pragma once
#include <komaru/editor/gui.hpp>

#include <GLFW/glfw3.h>

class OSWindow {
public:
    explicit OSWindow(GLFWwindow* glfw_window);

    ImVec2 GetSize() const;

private:
    GLFWwindow* glfw_window_;
};
