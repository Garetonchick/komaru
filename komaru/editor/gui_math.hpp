#pragma once
#include <editor/gui.hpp>

static constexpr float kEps = 1e-5;

float Length(ImVec2 v);
float IsNearZero(float x);
float IsNearZero(ImVec2 v);
bool VecEq(ImVec2 a, ImVec2 b);
bool FloatEq(float a, float b);
