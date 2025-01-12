#pragma once

namespace ImGui {

class IdGuard {
public:
    explicit IdGuard(int id);
    ~IdGuard();
};

}
