#include "node.hpp"

#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QFocusEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

namespace komaru::editor {

Node::Node(QGraphicsItem* parent)
    : QGraphicsObject(parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsFocusable);

    SetupMainText();

    connect(main_text_->document(), &QTextDocument::contentsChanged, this, &Node::UpdateLayout);
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

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    QPointF local_pos = main_text_->mapFromParent(event->pos());
    if (main_text_->shape().contains(local_pos)) {
        StartMainTextEditing();
        event->accept();
        return;
    }

    QGraphicsObject::mouseDoubleClickEvent(event);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemSelectedChange && !value.toBool()) {
        StopMainTextEditing();
    } else if (change == QGraphicsItem::ItemSceneHasChanged) {
        if (value.toBool() && scene()) {
            main_text_->setTextInteractionFlags(Qt::NoTextInteraction);
            scene()->update();
        }
    }

    return QGraphicsObject::itemChange(change, value);
}

void Node::focusOutEvent(QFocusEvent* event) {
    if (!main_text_->hasFocus()) {
        StopMainTextEditing();
    }

    QGraphicsObject::focusOutEvent(event);
}

void Node::SetupMainText() {
    main_text_ = new QGraphicsTextItem(this);
    main_text_->setDefaultTextColor(Qt::lightGray);
    QFont font = main_text_->font();
    font.setPointSize(20);
    main_text_->setFont(font);
    main_text_->setTextInteractionFlags(Qt::NoTextInteraction);
    main_text_->setFlag(QGraphicsItem::ItemIsFocusable);
    main_text_->setPlainText("Int");

    QTextOption text_option = main_text_->document()->defaultTextOption();
    text_option.setAlignment(Qt::AlignCenter);
    main_text_->document()->setDefaultTextOption(text_option);
    main_text_->document()->setDocumentMargin(4);
    main_text_->setTextInteractionFlags(Qt::NoTextInteraction);

    UpdateLayout();
}

void Node::StartMainTextEditing() {
    main_text_->setTextInteractionFlags(Qt::TextEditorInteraction);
    main_text_->setFocus(Qt::MouseFocusReason);
    main_text_->setTextInteractionFlags(Qt::TextEditorInteraction);

    QTextCursor text_cursor = main_text_->textCursor();
    text_cursor.select(QTextCursor::Document);
    main_text_->setTextCursor(text_cursor);
}

void Node::StopMainTextEditing() {
    main_text_->setTextInteractionFlags(Qt::NoTextInteraction);
    QTextCursor cursor = main_text_->textCursor();
    cursor.clearSelection();
    cursor.setPosition(0);
    main_text_->setTextCursor(cursor);
    main_text_->clearFocus();
    main_text_->update();

    UpdateLayout();
}

void Node::UpdateLayout() {
    QRectF main_text_rect = main_text_->boundingRect();

    prepareGeometryChange();
    bounding_rect_ = QRectF(0, 0, main_text_rect.width() + 2 * kMainTextPadding,
                            main_text_rect.height() + 2 * kMainTextPadding);
    main_text_->setPos(kMainTextPadding, kMainTextPadding);
    update();
}

}  // namespace komaru::editor
