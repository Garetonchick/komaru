#pragma once
#include <editor/gui.hpp>

namespace ImGui {

class ChildGuard {
public:
    ChildGuard(const char* str_id, const ImVec2& size = ImVec2(0, 0),
               ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    ChildGuard(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0,
               ImGuiWindowFlags window_flags = 0);
    ~ChildGuard();
};

}  // namespace ImGui
