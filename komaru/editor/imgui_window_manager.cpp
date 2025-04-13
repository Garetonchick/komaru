#include "imgui_window_manager.hpp"

#include <editor/gui_math.hpp>

ImguiWindowManager::ImguiWindowManager(ImVec2 pos, ImVec2 size)
    : pos_(pos),
      size_(size) {
}

ImVec2 ImguiWindowManager::GetPos() const {
    return pos_;
}

ImVec2 ImguiWindowManager::GetSize() const {
    return size_;
}

ImVec2 ImguiWindowManager::PropogateMove() {
    auto actual_pos = ImGui::GetWindowPos();
    auto round_pos = Round(pos_);
    if (!VecEq(round_pos, actual_pos)) {
        auto move = actual_pos - round_pos;
        pos_ += move;
        return move;
    }
    return {0.f, 0.f};
}

ImVec2 ImguiWindowManager::PropogateResize() {
    auto actual_size = ImGui::GetWindowSize();
    auto round_size = Round(size_);
    if (!VecEq(round_size, actual_size)) {
        auto size_move = actual_size - round_size;
        size_ += size_move;
        return size_move;
    }
    return {0.f, 0.f};
}

void ImguiWindowManager::SetPos(ImVec2 pos) {
    if (!VecEq(Round(pos), Round(pos_))) {
        ImGui::SetWindowPos(Round(pos), ImGuiCond_Always);
        pos_ = pos;
    } else if (!VecEq(pos_, pos)) {
        pos_ = pos;
    }
}

void ImguiWindowManager::SetSize(ImVec2 size) {
    if (!VecEq(Round(size), Round(size_))) {
        ImGui::SetWindowSize(Round(size), ImGuiCond_Always);
        size_ = size;
    } else if (!VecEq(size_, size)) {
        size_ = size;
    }
}
