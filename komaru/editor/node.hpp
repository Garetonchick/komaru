#pragma once

#include <QGraphicsObject>

#include <komaru/editor/pin.hpp>
#include <komaru/editor/text.hpp>

namespace komaru::editor {

class Node : public QGraphicsObject {
    Q_OBJECT

public:
    explicit Node(QGraphicsItem* parent = nullptr);
    ~Node();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    Pin* GetInputPin();
    std::vector<Pin*>& GetOutputPins();
    void EnableLabels();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    void SetupMainText();
    void StartMainTextEditing();
    void StopMainTextEditing();
    void UpdateLayout();
    void SetNewInputPin();
    bool RemoveInputPin();
    void AddOutputPin();
    bool RemoveOutputPin();
    void PositionPins();
    void SetupPinLabelText(Pin* pin);
    void StartPinLabelTextEditing(Text* text);
    void StopPinLabelTextEditing(Text* text);
    void StopAllTextEditingAndUpdate();
    void PositionPinLabels();

private:
    static constexpr QColor kNodeColor{60, 60, 60};
    static constexpr QColor kOutlineColor{120, 120, 120};
    static constexpr QColor kSelectedOutlineColor{5, 119, 227};
    static constexpr qreal kOutlineWidth{2.0};
    static constexpr qreal kRoundingRadius{5.0};
    static constexpr qreal kMainTextPadding{10.f};
    static constexpr qreal kHeightPerPin{30.f};
    static constexpr qreal kPinLabelRightPadding{5.f};

    QRectF bounding_rect_{0, 0, 100, 100};
    Text* main_text_{nullptr};
    Pin* input_pin_{nullptr};
    std::vector<Pin*> output_pins_;
    std::unordered_map<Pin*, Text*> pin2label_;
};

}  // namespace komaru::editor
