#pragma once

#include <QGraphicsTextItem>

namespace komaru::editor {

class Text : public QGraphicsTextItem {
    Q_OBJECT

public:
    using QGraphicsTextItem::QGraphicsTextItem;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void FocusedOut(Text* text);
};

}  // namespace komaru::editor
