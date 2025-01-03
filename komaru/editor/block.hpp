#pragma once

#include <editor/gui.hpp>

#include <string>

class Block {
public:
    explicit Block(
        std::string name,
        ImVec2 pos = {0, 0},
        ImVec2 size = {100, 100},
        ImVec2 pivot={0, 0}
    );

    void UpdateAndDraw(float dt);
private:
    void BeforeWindow();
    void Update(float dt);
    void Draw();

    std::string name_;
    ImVec2 pos_;
    ImVec2 pivot_;
    ImVec2 size_;
    bool disable_resize_{false};
};
