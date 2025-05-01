#include "pin.hpp"

#include <QPainter>

#include <komaru/editor/node.hpp>

namespace komaru::editor {

Pin::Pin(Node* node, PinType type, QGraphicsItem* parent)
    : QGraphicsItem(parent),
      node_(node),
      type_(type) {
    const qreal radius = 5.f;
    bounding_rect_ = QRectF(-radius, -radius, 2 * radius, 2 * radius);
}

QRectF Pin::boundingRect() const {
    return bounding_rect_;
}

void Pin::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    const QColor color = (type_ == Input) ? Qt::cyan : Qt::magenta;
    painter->setBrush(color);
    painter->setPen(QPen(Qt::black, isSelected() ? 1.5 : 0.5));
    painter->drawEllipse(bounding_rect_);
}

}  // namespace komaru::editor
