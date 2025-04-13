#include "imgui_child_guard.hpp"

#include <editor/gui.hpp>

namespace ImGui {

ChildGuard::ChildGuard(const char* str_id, const ImVec2& size, ImGuiChildFlags child_flags,
                       ImGuiWindowFlags window_flags) {
    ImGui::BeginChild(str_id, size, child_flags, window_flags);
}

ChildGuard::ChildGuard(ImGuiID id, const ImVec2& size, ImGuiChildFlags child_flags,
                       ImGuiWindowFlags window_flags) {
    ImGui::BeginChild(id, size, child_flags, window_flags);
}

ChildGuard::~ChildGuard() {
    ImGui::EndChild();
}

}  // namespace ImGui
