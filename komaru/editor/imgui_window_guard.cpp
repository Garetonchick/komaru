#include "imgui_window_guard.hpp"

#include <print>

ImGuiWindowGuard::ImGuiWindowGuard(const char* window_name, bool* p_open, ImGuiWindowFlags flags) {
    ImGui::Begin(window_name, p_open, flags);
}

ImGuiWindowGuard::~ImGuiWindowGuard() {
    ImGui::End();
}
