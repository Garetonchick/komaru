#include "imgui_id_guard.hpp"

#include <komaru/editor/gui.hpp>

namespace ImGui {

IdGuard::IdGuard(int id) {
    ImGui::PushID(id);
}

IdGuard::~IdGuard() {
    ImGui::PopID();
}

}  // namespace ImGui
