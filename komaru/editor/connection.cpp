#include "connection.hpp"

#include <QPen>

namespace komaru::editor {

Connection::Connection(Pin* source, Pin* target, QGraphicsItem* parent)
    : QGraphicsPathItem(parent),
      source_pin_(source),
      target_pin_(target) {
    setPen(QPen(Qt::green, 2));
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    UpdateLayout();

    source_pin_->AddConnection(this);
    target_pin_->AddConnection(this);
}

bool Connection::HasPin(const Pin* pin) const {
    return pin == source_pin_ || pin == target_pin_;
}

void Connection::UpdateLayout() {
    QPointF start = source_pin_->mapToScene(0, 0);
    QPointF end = target_pin_->mapToScene(0, 0);

    QPainterPath path;
    path.moveTo(start);

    qreal dx = target_pin_->x() - source_pin_->x();
    QPointF ctrl1 = start + QPointF(dx * 0.5, 0);
    QPointF ctrl2 = end - QPointF(dx * 0.5, 0);

    qreal dist = qAbs(dx);
    ctrl1.setX(start.x() + qMax(dist * 0.5, 20.0));
    ctrl2.setX(end.x() - qMax(dist * 0.5, 20.0));

    path.cubicTo(ctrl1, ctrl2, end);

    setPath(path);
}

void Connection::Detach() {
    source_pin_->RemoveConnection(this);
    target_pin_->RemoveConnection(this);
}

}  // namespace komaru::editor
