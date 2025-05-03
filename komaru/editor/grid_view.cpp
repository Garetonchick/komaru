#include "grid_view.hpp"

#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QPushButton>
#include <QStyle>

#include <komaru/editor/node.hpp>
#include <komaru/editor/connection.hpp>

namespace komaru::editor {

GridView::GridView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setSceneRect(-1'000'000, -1'000'000, 2'000'000, 2'000'000);
    setMouseTracking(true);

    SetupToolbar();
}

void GridView::SetLineGap(qreal gap) {
    line_gap_ = gap;
}

void GridView::SetBaseColor(QColor color) {
    base_color_ = color;
}

void GridView::SetBaseWidth(qreal width) {
    base_width_ = width;
}

void GridView::SetMajorWidthMul(qreal mul) {
    major_width_mul_ = mul;
}

void GridView::SetMajorDarkerFactor(int factor) {
    major_darker_factor_ = factor;
}

void GridView::SetMajorMod(size_t mod) {
    major_mod_ = mod;
}

void GridView::SetZoomSpeed(qreal speed) {
    zoom_speed_ = speed;
}

void GridView::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsView::drawBackground(painter, rect);

    qreal vert_start = std::floor(rect.left() / line_gap_) * line_gap_;
    qreal vert_finish = std::ceil(rect.right() / line_gap_) * line_gap_;
    qreal hor_start = std::floor(rect.top() / line_gap_) * line_gap_;
    qreal hor_finish = std::ceil(rect.bottom() / line_gap_) * line_gap_;

    auto get_pen = [this](qreal pos) -> QPen {
        bool is_major = (std::lround(std::abs(pos) / line_gap_) % major_mod_) == 0;
        if (is_major) {
            return QPen(base_color_.darker(major_darker_factor_), base_width_ * major_width_mul_);
        }

        return QPen(base_color_, base_width_);
    };

    for (qreal x = vert_start; x <= vert_finish; x += line_gap_) {
        painter->setPen(get_pen(x));
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    for (qreal y = hor_start; y <= hor_finish; y += line_gap_) {
        painter->setPen(get_pen(y));
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void GridView::wheelEvent(QWheelEvent* event) {
    if (!(event->modifiers() & Qt::ControlModifier)) {
        QPoint last_scroll_pos(horizontalScrollBar()->value(), verticalScrollBar()->value());
        disable_scrolling_ = true;
        QGraphicsView::wheelEvent(event);
        disable_scrolling_ = false;
        horizontalScrollBar()->setValue(last_scroll_pos.x());
        verticalScrollBar()->setValue(last_scroll_pos.y());
        return;
    }
    if (is_panning_) {
        event->accept();
        return;
    }

    if (event->angleDelta().y() < 0) {
        Zoom(1.0 / zoom_speed_);
    } else {
        Zoom(zoom_speed_);
    }

    event->accept();
}

void GridView::scrollContentsBy(int dx, int dy) {
    if (disable_scrolling_) {
        return;
    }
    QGraphicsView::scrollContentsBy(dx, dy);
}

void GridView::Zoom(qreal mul) {
    scale(mul, mul);
}

void GridView::SetupToolbar() {
    toolbar_ = new QToolBar(this);
    toolbar_->setMovable(false);
    toolbar_->setFloatable(false);
    toolbar_->setStyleSheet(
        "QToolBar { background-color: rgba(240, 240, 240, 220); border-radius: 4px; border: 1px "
        "solid darkgray; }");

    auto* run_button = new QPushButton(toolbar_);
    auto* save_button = new QPushButton(toolbar_);
    auto* load_button = new QPushButton(toolbar_);

    run_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    save_button->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    load_button->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));

    run_button->setToolTip("Run");
    save_button->setToolTip("Save");
    load_button->setToolTip("Load");

    QString button_style =
        "QPushButton { min-width: 40px; min-height: 40px; padding: 5px; margin: 2px; "
        "background-color: #f0f0f0; border: 1px solid #c0c0c0; border-radius: 3px; }"
        "QPushButton:hover { background-color: #e0e0e0; }"
        "QPushButton:pressed { background-color: #d0d0d0; }";

    run_button->setStyleSheet(button_style);
    save_button->setStyleSheet(button_style);
    load_button->setStyleSheet(button_style);

    run_button->setIconSize(QSize(icon_size_, icon_size_));
    save_button->setIconSize(QSize(icon_size_, icon_size_));
    load_button->setIconSize(QSize(icon_size_, icon_size_));

    toolbar_->addWidget(run_button);
    toolbar_->addWidget(save_button);
    toolbar_->addWidget(load_button);

    connect(run_button, &QPushButton::clicked, this, &GridView::OnRunAction);
    connect(save_button, &QPushButton::clicked, this, &GridView::OnSaveAction);
    connect(load_button, &QPushButton::clicked, this, &GridView::OnLoadAction);

    PositionToolbar();
}

void GridView::PositionToolbar() {
    int x = (width() - toolbar_->sizeHint().width()) / 2;
    toolbar_->move(x, 10);
    toolbar_->raise();
}

void GridView::OnRunAction() {
    std::println("OnRunAction");
}

void GridView::OnSaveAction() {
    std::println("OnSaveAction");
}

void GridView::OnLoadAction() {
    std::println("OnLoadAction");
}

void GridView::mousePressEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());

    if (event->button() == Qt::RightButton) {
        is_panning_ = true;
        last_pan_pos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);

        event->accept();
        return;
    } else if (event->button() == Qt::LeftButton && item) {
        if (Pin* pin = dynamic_cast<Pin*>(item)) {
            QPointF pin_pos = pin->mapToScene(0, 0);
            QPointF cursor_pos = mapToScene(event->pos());
            conn_start_pin_ = pin;

            pending_conn_ = new QGraphicsLineItem();
            pending_conn_->setPen(QPen(Qt::yellow, 2));
            pending_conn_->setLine(QLineF(pin_pos, cursor_pos));
            pending_conn_->setZValue(1000);

            scene()->addItem(pending_conn_);
            setCursor(Qt::CrossCursor);
            event->accept();
            return;
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GridView::mouseMoveEvent(QMouseEvent* event) {
    if (is_panning_) {
        QPoint dlt = event->pos() - last_pan_pos_;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dlt.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - dlt.y());

        last_pan_pos_ = event->pos();

        event->accept();
    } else if (pending_conn_) {
        QPointF cursor_pos = mapToScene(event->pos());
        QPointF pin_pos = conn_start_pin_->mapToScene(0, 0);
        pending_conn_->setLine(QLineF(pin_pos, cursor_pos));
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GridView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton && is_panning_) {
        is_panning_ = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton && pending_conn_) {
        setCursor(Qt::ArrowCursor);

        scene()->removeItem(pending_conn_);
        delete pending_conn_;
        pending_conn_ = nullptr;

        QGraphicsItem* item = itemAt(event->pos());
        Pin* end_pin = dynamic_cast<Pin*>(item);

        if (end_pin && conn_start_pin_->ConnectableTo(end_pin)) {
            Pin* output_pin = conn_start_pin_;
            Pin* input_pin = end_pin;

            if (output_pin->GetPinType() != Pin::Output) {
                std::swap(output_pin, input_pin);
            }

            Connection* conn = new Connection(output_pin, input_pin);
            scene()->addItem(conn);
        }

        conn_start_pin_ = nullptr;
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void GridView::keyPressEvent(QKeyEvent* event) {
    bool ctrl = event->modifiers() & Qt::ControlModifier;
    if (event->key() == Qt::Key_A && ctrl) {
        QPointF cursor_pos = mapToScene(mapFromGlobal(QCursor::pos()));

        auto node = std::make_unique<Node>();
        node->setPos(cursor_pos);
        scene()->addItem(node.release());
        event->accept();
    } else if (event->key() == Qt::Key_D && ctrl) {
        QList<QGraphicsItem*> selected = scene()->selectedItems();

        for (QGraphicsItem* item : selected) {
            if (Node* node = dynamic_cast<Node*>(item)) {
                scene()->removeItem(node);
                delete node;
            } else if (Connection* conn = dynamic_cast<Connection*>(item)) {
                conn->Detach();
                scene()->removeItem(conn);
                delete conn;
            }
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GridView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    PositionToolbar();
}

}  // namespace komaru::editor
