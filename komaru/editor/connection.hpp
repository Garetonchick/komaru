#pragma once

#include <QGraphicsPathItem>

#include <komaru/editor/pin.hpp>

namespace komaru::editor {

class Connection : public QGraphicsPathItem {
public:
    Connection(Pin* source, Pin* target, QGraphicsItem* parent = nullptr);

    bool HasPin(const Pin* pin) const;
    void UpdateLayout();
    void Detach();

private:
    Pin* source_pin_;
    Pin* target_pin_;
};

}  // namespace komaru::editor
