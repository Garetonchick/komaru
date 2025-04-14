#pragma once
#include <komaru/editor/gui.hpp>

static constexpr float kEps = 1e-5;

float Length(ImVec2 v);
float IsNearZero(float x);
float IsNearZero(ImVec2 v);
bool VecEq(ImVec2 a, ImVec2 b);
bool FloatEq(float a, float b);
ImVec2 Truncate(ImVec2 v);
ImVec2 Round(ImVec2 v);
ImVec2 Floor(ImVec2 v);
ImVec2 Ceil(ImVec2 v);
