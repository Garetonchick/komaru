#pragma once
#include <editor/gui_element.hpp>
#include <editor/top_down_camera.hpp>
#include <editor/node.hpp>
#include <editor/link.hpp>

#include <string>
#include <vector>

namespace komaru::editor {

class NodeEditor : public GuiElement {
public:
    explicit NodeEditor(
        std::string name,
        ImVec2 pos = {0, 0},
        ImVec2 size = {800, 800}
    );

    void UpdateAndDraw(float dt) override;

private:
    Node* GetNodeByID(int id);
    Node* GetNodeByInID(int in_id);
    Node* GetNodeByOutID(int out_id);
    bool DestroyLinkByID(int link_id);

private:
    std::string name_;
    ImVec2 pos_;
    ImVec2 size_;

    TopDownCamera camera_;
    std::vector<Node> nodes_;
    std::vector<Link> links_;
};

}
