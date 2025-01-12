#pragma once

namespace ImGui {
    inline int AllocateID() {
        static int id = 1;
        return id++;
    }
}
