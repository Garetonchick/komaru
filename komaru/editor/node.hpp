#pragma once
#include <editor/gui.hpp>
#include <editor/gui_element.hpp>

#include <string>

namespace komaru::editor {

class Node : public GuiElement {
public:
    Node(std::string title, ImVec2 pos);

    void UpdateAndDraw(float dt) override;

    bool IsMaybeSelected() const;

private:
    static constexpr ImU32 kNodeColor = IM_COL32(42, 87, 100, 255);

    int id_{0};
    std::string title_;
    ImVec2 pos_;
    ImVec2 size_;
    float v_{0.f};
    bool maybe_selected_{false};
};

}
