#pragma once

#include <QGraphicsItem>

namespace komaru::editor {

class Node;

class Pin : public QGraphicsItem {
public:
    enum PinType { Input, Output };

public:
    Pin(Node* node, PinType type, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

private:
    [[maybe_unused]] Node* node_;
    PinType type_;
    QRectF bounding_rect_;
};

}  // namespace komaru::editor
