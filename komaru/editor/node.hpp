#pragma once
#include <editor/gui.hpp>
#include <editor/gui_element.hpp>

#include <string>

namespace komaru::editor {

class Node : public GuiElement {
public:
    Node(std::string type, ImVec2 pos);

    void UpdateAndDraw(float dt) override;

private:
    int id_{0};
    int in_id_{0};
    int out_id_{0};
    std::string type_;
    ImVec2 pos_;
    ImVec2 size_;
};

}
