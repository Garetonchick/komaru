#pragma once
#include <editor/gui.hpp>

class TopDownCamera {
public:
    TopDownCamera() = default;

    void SetScale(float scale);
    void SetPos(ImVec2 pos);
    void Move(ImVec2 step);

    ImVec2 Global2Camera(ImVec2 v) const;
    ImVec2 Camera2Global(ImVec2 v) const;
    ImVec2 GetPos() const;
    float GetScale() const;

private:
    float scale_{1.f};
    ImVec2 pos_{0.f, 0.f};  // upper left corner of the camera
};
