#include "imgui_window_guard.hpp"

namespace ImGui {

WindowGuard::WindowGuard(const char* window_name, bool* p_open, ImGuiWindowFlags flags) {
    skip_ = !ImGui::Begin(window_name, p_open, flags);
}

bool WindowGuard::Skip() {
    return skip_;
}

WindowGuard::~WindowGuard() {
    ImGui::End();
}

}  // namespace ImGui
