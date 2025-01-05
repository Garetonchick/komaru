#include "top_down_camera.hpp"

void TopDownCamera::SetScale(float scale) {
    scale_ = scale;
}

void TopDownCamera::SetPos(ImVec2 pos) {
    pos_ = pos;
}

void TopDownCamera::Move(ImVec2 step) {
    pos_ += step;
}

ImVec2 TopDownCamera::Global2Camera(ImVec2 v) const {
    return (v - pos_) * scale_;
}

ImVec2 TopDownCamera::Camera2Global(ImVec2 v) const {
    return pos_ + v / scale_;
}

ImVec2 TopDownCamera::GetPos() const {
    return pos_;
}

float TopDownCamera::GetScale() const {
    return scale_;
}
