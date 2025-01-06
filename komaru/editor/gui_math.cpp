#include "gui_math.hpp"

#include <cmath>

float Length(ImVec2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float IsNearZero(float x) {
    return std::abs(x) < kEps;
}

float IsNearZero(ImVec2 v) {
    return Length(v) < kEps;
}

bool VecEq(ImVec2 a, ImVec2 b) {
    return IsNearZero(a - b);
}

bool FloatEq(float a, float b) {
    return std::abs(a - b) < kEps;
}

ImVec2 Truncate(ImVec2 v) {
    return {std::truncf(v.x), std::truncf(v.y)};
}

ImVec2 Round(ImVec2 v) {
    return {std::roundf(v.x), std::roundf(v.y)};
}

ImVec2 Floor(ImVec2 v) {
    return {std::floorf(v.x), std::floorf(v.y)};
}

ImVec2 Ceil(ImVec2 v) {
    return {std::ceilf(v.x), std::ceilf(v.y)};
}
