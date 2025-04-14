#pragma once
#include <komaru/editor/gui.hpp>

namespace ImGui {

class WindowGuard {
public:
    explicit WindowGuard(const char* window_name, bool* p_open = nullptr,
                         ImGuiWindowFlags flags = 0);

    bool Skip();

    ~WindowGuard();

private:
    bool skip_{false};
};

}  // namespace ImGui
