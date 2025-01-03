#pragma once
#include <editor/gui.hpp>

class ImGuiWindowGuard {
public:
    explicit ImGuiWindowGuard(const char* window_name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0);
    ~ImGuiWindowGuard();
};
