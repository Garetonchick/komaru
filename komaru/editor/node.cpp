#include "node.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace komaru::editor {

Node::Node(QGraphicsItem* parent)
    : QGraphicsObject(parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setCacheMode(DeviceCoordinateCache);
}

QRectF Node::boundingRect() const {
    return bounding_rect_;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setBrush(kNodeColor);
    painter->setPen(QPen(kOutlineColor, kOutlineWidth));
    painter->drawRoundedRect(bounding_rect_, kRoundingRadius, kRoundingRadius);

    if (option->state & QStyle::State_HasFocus) {
        painter->setPen(QPen(kSelectedOutlineColor, kOutlineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(bounding_rect_, kRoundingRadius, kRoundingRadius);
    }
}

}  // namespace komaru::editor
