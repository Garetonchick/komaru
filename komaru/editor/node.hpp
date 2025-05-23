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

    void SetMainText(const QString& s);
    void SetTagText(const QString& s);
    void SetPinLabel(Pin* pin, const QString& s);
    void SetNewInputPin();
    bool RemoveInputPin();
    void EnableLabels();
    void AddOutputPin();
    bool RemoveOutputPin();

    Pin* GetInputPin();
    std::vector<Pin*>& GetOutputPins();
    const Text* GetMainText() const;
    const Text* GetTagText() const;
    const Text* GetPinLabel(Pin* pin) const;

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
    void PositionPins();
    void SetupPinLabelText(Pin* pin);
    void StartPinLabelTextEditing(Text* text);
    void StopPinLabelTextEditing(Text* text);
    void StopAllTextEditingAndUpdate();
    void PositionPinLabels();
    void SetupTagText();
    void StartTagTextEditing();
    void StopTagTextEditing();
    void PositionTag();

private:
    static constexpr QColor kNodeColor{60, 60, 60};
    static constexpr QColor kOutlineColor{120, 120, 120};
    static constexpr QColor kSelectedOutlineColor{5, 119, 227};
    static constexpr qreal kOutlineWidth{2.0};
    static constexpr qreal kRoundingRadius{5.0};
    static constexpr qreal kMainTextPadding{10.f};
    static constexpr qreal kHeightPerPin{30.f};
    static constexpr qreal kPinLabelRightPadding{5.f};
    static constexpr qreal kTagMargin{5.f};

    QRectF bounding_rect_{0, 0, 100, 100};
    Text* main_text_{nullptr};
    Text* tag_text_{nullptr};
    Pin* input_pin_{nullptr};
    std::vector<Pin*> output_pins_;
    std::unordered_map<Pin*, Text*> pin2label_;
};

}  // namespace komaru::editor
