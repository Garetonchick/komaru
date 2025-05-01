#include "text.hpp"

#include <QStyle>
#include <QStyleOptionGraphicsItem>

namespace komaru::editor {

void Text::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QStyleOptionGraphicsItem modified_options = *option;

    modified_options.state &= ~QStyle::State_Selected;
    modified_options.state &= ~QStyle::State_HasFocus;

    QGraphicsTextItem::paint(painter, &modified_options, widget);
}

}  // namespace komaru::editor
