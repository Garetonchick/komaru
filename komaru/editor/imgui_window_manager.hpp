#pragma once
#include <komaru/editor/gui.hpp>

class ImguiWindowManager {
public:
    ImguiWindowManager(ImVec2 pos, ImVec2 size);

    ImVec2 GetPos() const;
    ImVec2 GetSize() const;

    ImVec2 PropogateMove();
    ImVec2 PropogateResize();

    void SetPos(ImVec2 pos);
    void SetSize(ImVec2 size);

private:
    ImVec2 pos_;
    ImVec2 size_;
};
