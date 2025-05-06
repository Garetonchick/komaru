#pragma once

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>

#include <komaru/editor/pin.hpp>
#include <komaru/editor/text.hpp>

namespace komaru::editor {

class Connection : public QObject,
                   public QGraphicsPathItem {
    Q_OBJECT

public:
    Connection(Pin* source, Pin* target, QGraphicsItem* parent = nullptr);

    bool HasPin(const Pin* pin) const;
    const Text* GetText() const;
    const Pin* GetSourcePin() const;
    const Pin* GetTargetPin() const;

    void SetText(const QString& s);
    void UpdateLayout();
    void Detach();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void SetupText();
    void PositionText();
    void StopEditing();

private:
    Pin* source_pin_;
    Pin* target_pin_;
    Text* text_;
};

}  // namespace komaru::editor
