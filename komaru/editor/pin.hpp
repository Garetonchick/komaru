#pragma once

#include <QGraphicsItem>

namespace komaru::editor {

class Node;
class Connection;

class Pin : public QGraphicsItem {
public:
    enum PinType { Input, Output };

public:
    Pin(Node* node, PinType type, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    void AddConnection(Connection* conn);
    void RemoveConnection(Connection* conn);
    void UpdateConnections();
    void DestroyConnections();

    bool ConnectableTo(const Pin* o) const;
    PinType GetPinType() const;

private:
    [[maybe_unused]] Node* node_;
    PinType type_;
    QRectF bounding_rect_;
    std::vector<Connection*> connections_;
};

}  // namespace komaru::editor
