#pragma once
#include <komaru/editor/gui.hpp>
#include <komaru/editor/gui_element.hpp>

#include <string>

namespace komaru::editor {

enum class NodeType {
    Normal,
    Start,
    Terminal,
};

class Node : public GuiElement {
public:
    Node(std::string label, ImVec2 pos, NodeType type = NodeType::Normal);

    void UpdateAndDraw(float dt) override;

    int GetID() const;
    int GetInPinID() const;
    int GetOutPinID() const;

private:
    int id_{0};
    int in_id_{0};
    int out_id_{0};
    std::string label_;
    ImVec2 pos_;
    ImVec2 size_;
    NodeType type_;
};

}  // namespace komaru::editor
