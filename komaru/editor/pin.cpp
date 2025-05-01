#include "pin.hpp"

#include <QPainter>
#include <QGraphicsScene>

#include <komaru/editor/node.hpp>
#include <komaru/editor/connection.hpp>
#include <komaru/util/std_extensions.hpp>

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

void Pin::AddConnection(Connection* conn) {
    connections_.push_back(conn);
}
void Pin::RemoveConnection(Connection* conn) {
    util::UnstableErase(connections_, conn);
}

void Pin::UpdateConnections() {
    for (Connection* conn : connections_) {
        conn->UpdateLayout();
    }
}

void Pin::DestroyConnections() {
    std::vector<Connection*> conns = std::move(connections_);
    for (Connection* conn : conns) {
        if (scene()) {
            scene()->removeItem(conn);
        }
        conn->Detach();
        delete conn;
    }
}

bool Pin::ConnectableTo(const Pin* o) const {
    if (type_ == o->type_) {
        return false;
    }
    for (Connection* conn : connections_) {
        if (conn->HasPin(o)) {
            return false;
        }
    }
    return true;
}

Pin::PinType Pin::GetPinType() const {
    return type_;
}

}  // namespace komaru::editor
