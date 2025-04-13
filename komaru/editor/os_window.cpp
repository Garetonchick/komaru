#include "os_window.hpp"

OSWindow::OSWindow(GLFWwindow* glfw_window)
    : glfw_window_(glfw_window) {
}

ImVec2 OSWindow::GetSize() const {
    int width, height;
    glfwGetWindowSize(glfw_window_, &width, &height);
    return {static_cast<float>(width), static_cast<float>(height)};
}
