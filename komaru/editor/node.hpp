#pragma once

#include <QGraphicsObject>

namespace komaru::editor {

class Node : public QGraphicsObject {
    Q_OBJECT

public:
    explicit Node(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    static constexpr QColor kNodeColor{60, 60, 60};
    static constexpr QColor kOutlineColor{120, 120, 120};
    static constexpr QColor kSelectedOutlineColor{5, 119, 227};
    static constexpr qreal kOutlineWidth{2.0};
    static constexpr qreal kRoundingRadius{5.0};

    QRectF bounding_rect_{0, 0, 100, 100};
};

}  // namespace komaru::editor
