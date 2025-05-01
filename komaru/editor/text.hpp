#pragma once

#include <QGraphicsTextItem>

namespace komaru::editor {

class Text : public QGraphicsTextItem {
public:
    using QGraphicsTextItem::QGraphicsTextItem;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
};

}  // namespace komaru::editor
