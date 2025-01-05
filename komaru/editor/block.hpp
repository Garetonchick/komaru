#pragma once

#include <editor/gui.hpp>
#include <editor/top_down_camera.hpp>
#include <editor/gui_element.hpp>

#include <string>

class Block : public GuiElement {
public:
    explicit Block(
        std::string name,
        const TopDownCamera* camera,
        ImVec2 pos = {0, 0},
        ImVec2 size = {100, 100},
        ImVec2 pivot={0, 0}
    );

    void UpdateAndDraw(float dt) override;
private:
    void BeforeWindow();
    void Update(float dt);
    void Draw();

    std::string name_;
    ImVec2 global_pos_;
    ImVec2 global_size_;
    ImVec2 initial_pivot_;
    bool disable_resize_{false};
    ImVec2 local_size_{0.f, 0.f};
    ImVec2 local_pos_{0.f, 0.f};
    const TopDownCamera* camera_;
};
