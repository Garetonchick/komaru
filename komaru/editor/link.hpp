#pragma once
#include <editor/node.hpp>

#include <string>

namespace komaru::editor {

class Link : GuiElement {
public:
    Link(const Node& from, const Node& to, std::string label="");

    void UpdateAndDraw(float dt) override;

    int GetID() const;

private:
    int id_{-1};
    int from_id_{-1};
    int to_id_{-1};
    std::string label_;
};

}
