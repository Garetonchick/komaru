#include "connection.hpp"

#include <QPen>
#include <QFont>
#include <QTextDocument>
#include <QTextOption>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>

namespace komaru::editor {

Connection::Connection(Pin* source, Pin* target, QGraphicsItem* parent)
    : QGraphicsPathItem(parent),
      source_pin_(source),
      target_pin_(target) {
    setPen(QPen(Qt::green, 2));
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    source_pin_->AddConnection(this);
    target_pin_->AddConnection(this);

    SetupText();
    UpdateLayout();

    connect(text_->document(), &QTextDocument::contentsChanged, this, &Connection::PositionText);
    connect(text_, &Text::FocusedOut, this, &Connection::StopEditing);
}

bool Connection::HasPin(const Pin* pin) const {
    return pin == source_pin_ || pin == target_pin_;
}

const Text* Connection::GetText() const {
    return text_;
}

const Pin* Connection::GetSourcePin() const {
    return source_pin_;
}

const Pin* Connection::GetTargetPin() const {
    return target_pin_;
}

void Connection::SetText(const QString& s) {
    text_->setPlainText(s);
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
    PositionText();
}

void Connection::Detach() {
    source_pin_->RemoveConnection(this);
    target_pin_->RemoveConnection(this);
}

void Connection::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    text_->setTextInteractionFlags(Qt::TextEditorInteraction);
    text_->setFocus();

    QTextCursor cursor = text_->textCursor();
    cursor.select(QTextCursor::Document);
    text_->setTextCursor(cursor);

    event->accept();
}

void Connection::SetupText() {
    text_ = new Text(this);
    text_->setPlainText("$");
    text_->setDefaultTextColor(Qt::black);
    text_->setTextInteractionFlags(Qt::TextEditorInteraction);

    QFont font = text_->font();
    font.setPointSize(12);
    text_->setFont(font);

    QTextOption text_option = text_->document()->defaultTextOption();
    text_option.setAlignment(Qt::AlignCenter);
    text_->document()->setDefaultTextOption(text_option);
}

void Connection::PositionText() {
    QPointF mid_point = shape().pointAtPercent(0.5);
    QPointF offset(0, -5);

    QRectF text_rect = text_->boundingRect();
    mid_point = mid_point - QPointF(text_rect.width() / 2, text_rect.height() + 5);
    text_->setPos(mid_point + offset);
}

void Connection::StopEditing() {
    QTextCursor cursor = text_->textCursor();
    cursor.clearSelection();
    cursor.setPosition(0);
    text_->setTextCursor(cursor);
    text_->update();
}

}  // namespace komaru::editor
