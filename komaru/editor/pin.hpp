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

    const std::vector<Connection*>& GetConnections() const;
    bool ConnectableTo(const Pin* o) const;
    PinType GetPinType() const;
    const Node* GetNode() const;

private:
    Node* node_;
    PinType type_;
    QRectF bounding_rect_;
    std::vector<Connection*> connections_;
};

}  // namespace komaru::editor
