#pragma once

class GuiElement {
public:
    virtual void UpdateAndDraw(float dt) = 0;
    virtual ~GuiElement() = default;
};
