#include "text.hpp"

#include <QStyle>
#include <QTextCursor>
#include <QStyleOptionGraphicsItem>

namespace komaru::editor {

void Text::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QStyleOptionGraphicsItem modified_options = *option;

    modified_options.state &= ~QStyle::State_Selected;
    modified_options.state &= ~QStyle::State_HasFocus;

    QGraphicsTextItem::paint(painter, &modified_options, widget);
}

void Text::focusOutEvent(QFocusEvent* event) {
    QGraphicsTextItem::focusOutEvent(event);
    emit FocusedOut();
}

void Text::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsTextItem::mouseDoubleClickEvent(event);

    if (toPlainText() == "$") {
        QTextCursor text_cursor = textCursor();
        text_cursor.select(QTextCursor::Document);
        setTextCursor(text_cursor);
    }
}

}  // namespace komaru::editor
