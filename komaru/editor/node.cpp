#include "node.hpp"

#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QFocusEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include <algorithm>

namespace komaru::editor {

Node::Node(QGraphicsItem* parent)
    : QGraphicsObject(parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsFocusable);

    SetupMainText();

    SetNewInputPin();
    AddOutputPin();
}

Node::~Node() {
    if (input_pin_) {
        input_pin_->DestroyConnections();
    }

    for (Pin* output_pin : output_pins_) {
        output_pin->DestroyConnections();
    }
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

Pin* Node::GetInputPin() {
    return input_pin_;
}

std::vector<Pin*>& Node::GetOutputPins() {
    return output_pins_;
}

void Node::EnableLabels() {
    if (output_pins_.size() != 1 || !pin2label_.empty()) {
        return;
    }

    SetupPinLabelText(output_pins_.front());
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    QPointF local_pos = main_text_->mapFromParent(event->pos());
    if (main_text_->shape().contains(local_pos)) {
        StartMainTextEditing();
        event->accept();
        return;
    }

    for (auto [_, label_text] : pin2label_) {
        QPointF local_pos = label_text->mapFromParent(event->pos());
        if (label_text->shape().contains(local_pos)) {
            StartPinLabelTextEditing(label_text);
            event->accept();
            return;
        }
    }

    QGraphicsObject::mouseDoubleClickEvent(event);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        if (input_pin_) {
            input_pin_->UpdateConnections();
        }
        for (Pin* pin : output_pins_) {
            pin->UpdateConnections();
        }
    } else if (change == QGraphicsItem::ItemSelectedChange && !value.toBool()) {
        StopAllTextEditingAndUpdate();
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
        StopAllTextEditingAndUpdate();
    }

    QGraphicsObject::focusOutEvent(event);
}

void Node::keyPressEvent(QKeyEvent* event) {
    bool ctrl = event->modifiers() & Qt::ControlModifier;
    if (event->key() == Qt::Key_I && ctrl) {
        if (input_pin_) {
            RemoveInputPin();
        } else {
            SetNewInputPin();
        }
        event->accept();
    } else if (event->key() == Qt::Key_E && ctrl) {
        if (output_pins_.size() != 1) {
            event->ignore();
            return;
        }
        EnableLabels();
        Text* text = pin2label_[output_pins_.front()];
        StartPinLabelTextEditing(text);
        event->accept();
    } else if (event->key() == Qt::Key_T && ctrl) {
        if (tag_text_) {
            StartTagTextEditing();
            event->accept();
            return;
        }
        SetupTagText();
        PositionTag();
        StartTagTextEditing();
        event->accept();
    } else {
        QGraphicsObject::keyPressEvent(event);
    }
}

void Node::wheelEvent(QGraphicsSceneWheelEvent* event) {
    if (event->modifiers() & Qt::ShiftModifier) {
        if (event->delta() > 0) {
            AddOutputPin();
        } else {
            RemoveOutputPin();
        }

        UpdateLayout();
        event->accept();
    } else {
        QGraphicsObject::wheelEvent(event);
    }
}

void Node::SetupMainText() {
    main_text_ = new Text(this);
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

    connect(main_text_->document(), &QTextDocument::contentsChanged, this, &Node::UpdateLayout);
    connect(main_text_, &Text::FocusedOut, this, &Node::StopMainTextEditing);

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
}

void Node::UpdateLayout() {
    QRectF main_text_rect = main_text_->boundingRect();
    qreal width = main_text_rect.width() + 2 * kMainTextPadding;
    qreal height = main_text_rect.height() + 2 * kMainTextPadding;
    height = std::max(height, output_pins_.size() * kHeightPerPin);

    for (auto [_, pin_text] : pin2label_) {
        QRectF text_rect = pin_text->boundingRect();
        width = std::max(width, main_text_rect.width() + 2 * kMainTextPadding + text_rect.width() +
                                    kPinLabelRightPadding);
    }

    prepareGeometryChange();
    bounding_rect_ = QRectF(0, 0, width, height);

    main_text_->setPos(kMainTextPadding, (height - main_text_rect.height()) * 0.5);
    PositionPinLabels();
    PositionPins();
    PositionTag();

    if (input_pin_) {
        input_pin_->UpdateConnections();
    }

    for (Pin* out_pin : output_pins_) {
        out_pin->UpdateConnections();
    }

    update();
}

void Node::SetNewInputPin() {
    RemoveInputPin();
    input_pin_ = new Pin(this, Pin::Input, this);
    PositionPins();
}

bool Node::RemoveInputPin() {
    if (!input_pin_) {
        return false;
    }
    scene()->removeItem(input_pin_);
    input_pin_->DestroyConnections();
    delete input_pin_;
    input_pin_ = nullptr;
    return true;
}

void Node::AddOutputPin() {
    output_pins_.push_back(new Pin(this, Pin::Output, this));

    if (output_pins_.size() > 1 || !pin2label_.empty()) {
        for (Pin* out_pin : output_pins_) {
            if (!pin2label_.contains(out_pin)) {
                SetupPinLabelText(out_pin);
            }
        }
    }

    UpdateLayout();
}

bool Node::RemoveOutputPin() {
    if (output_pins_.empty()) {
        return false;
    }
    Pin* pin = output_pins_.back();
    output_pins_.pop_back();

    auto label_it = pin2label_.find(pin);

    if (label_it != pin2label_.end()) {
        Text* label_text = label_it->second;
        pin2label_.erase(label_it);
        delete label_text;
    }

    pin->DestroyConnections();
    scene()->removeItem(pin);
    delete pin;
    return true;
}

void Node::PositionPins() {
    if (input_pin_) {
        input_pin_->setPos(0, bounding_rect_.height() * 0.5);
        input_pin_->setZValue(this->zValue() + 1);
    }

    if (output_pins_.empty()) {
        return;
    }

    qreal pin_step = bounding_rect_.height() / (output_pins_.size() + 1);
    qreal pin_y = pin_step;

    for (Pin* output_pin : output_pins_) {
        output_pin->setPos(bounding_rect_.width(), pin_y);
        output_pin->setZValue(this->zValue() + 1);
        pin_y += pin_step;
    }
}

void Node::SetupPinLabelText(Pin* pin) {
    Text* label_text = new Text(this);
    pin2label_[pin] = label_text;

    label_text->setDefaultTextColor(Qt::lightGray);
    QFont font = label_text->font();
    font.setPointSize(12);
    label_text->setFont(font);
    label_text->setTextInteractionFlags(Qt::NoTextInteraction);
    label_text->setFlag(QGraphicsItem::ItemIsFocusable);
    label_text->setPlainText("*");

    QTextOption text_option = label_text->document()->defaultTextOption();
    text_option.setAlignment(Qt::AlignLeft);
    label_text->document()->setDefaultTextOption(text_option);
    label_text->setTextInteractionFlags(Qt::NoTextInteraction);

    connect(label_text->document(), &QTextDocument::contentsChanged, this, &Node::UpdateLayout);
    connect(label_text, &Text::FocusedOut, this, &Node::StopPinLabelTextEditing);
}

void Node::StartPinLabelTextEditing(Text* text) {
    text->setTextInteractionFlags(Qt::TextEditorInteraction);
    text->setFocus(Qt::MouseFocusReason);
    text->setTextInteractionFlags(Qt::TextEditorInteraction);

    QTextCursor text_cursor = text->textCursor();
    text_cursor.select(QTextCursor::Document);
    text->setTextCursor(text_cursor);
}

void Node::StopPinLabelTextEditing(Text* text) {
    if (text->textInteractionFlags() & Qt::NoTextInteraction) {
        return;
    }
    text->setTextInteractionFlags(Qt::NoTextInteraction);
    QTextCursor cursor = text->textCursor();
    cursor.clearSelection();
    cursor.setPosition(0);
    text->setTextCursor(cursor);
    text->clearFocus();
    text->update();
}

void Node::StopAllTextEditingAndUpdate() {
    StopMainTextEditing();
    StopTagTextEditing();

    for (auto [_, label_text] : pin2label_) {
        StopPinLabelTextEditing(label_text);
    }

    UpdateLayout();
}

void Node::PositionPinLabels() {
    qreal pin_step = bounding_rect_.height() / (output_pins_.size() + 1);
    qreal pin_y = pin_step;

    for (Pin* pin : output_pins_) {
        auto it = pin2label_.find(pin);

        if (it == pin2label_.end()) {
            continue;
        }

        Text* text = it->second;
        QRectF text_rect = text->boundingRect();
        text->setPos(bounding_rect_.width() - kPinLabelRightPadding - text_rect.width(),
                     pin_y - text_rect.height() * 0.5);
        pin_y += pin_step;
    }
}

void Node::SetupTagText() {
    tag_text_ = new Text(this);
    tag_text_->setDefaultTextColor(Qt::black);
    QFont font = tag_text_->font();
    font.setPointSize(12);
    tag_text_->setFont(font);
    tag_text_->setTextInteractionFlags(Qt::TextEditorInteraction);

    connect(tag_text_->document(), &QTextDocument::contentsChanged, this, &Node::PositionTag);
    connect(tag_text_, &Text::FocusedOut, this, &Node::StopTagTextEditing);
}

void Node::StartTagTextEditing() {
    if (!tag_text_) {
        return;
    }

    tag_text_->setTextInteractionFlags(Qt::TextEditorInteraction);
    tag_text_->setFocus(Qt::MouseFocusReason);
    tag_text_->setTextInteractionFlags(Qt::TextEditorInteraction);

    QTextCursor text_cursor = tag_text_->textCursor();
    text_cursor.select(QTextCursor::Document);
    tag_text_->setTextCursor(text_cursor);
}

void Node::StopTagTextEditing() {
    if (!tag_text_) {
        return;
    }
    QTextCursor cursor = tag_text_->textCursor();
    cursor.clearSelection();
    cursor.setPosition(0);
    tag_text_->setTextCursor(cursor);
    tag_text_->clearFocus();
    tag_text_->update();
}

void Node::PositionTag() {
    if (!tag_text_) {
        return;
    }
    QRectF text_rect = tag_text_->boundingRect();
    tag_text_->setPos((bounding_rect_.width() - text_rect.width()) * 0.5,
                      -kTagMargin - text_rect.height());
}

}  // namespace komaru::editor
