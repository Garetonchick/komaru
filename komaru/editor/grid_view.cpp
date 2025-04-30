#include "grid_view.hpp"

#include <QWheelEvent>
#include <QScrollBar>

namespace komaru::editor {

GridView::GridView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(-1'000'000, -1'000'000, 2'000'000, 2'000'000);
    setMouseTracking(true);
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
        bool is_major = (std::lround(pos / line_gap_) % major_mod_) == 0;
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

void GridView::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::RightButton) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    is_panning_ = true;
    last_pan_pos_ = event->pos();
    setCursor(Qt::ClosedHandCursor);

    event->accept();
}

void GridView::mouseMoveEvent(QMouseEvent* event) {
    if (!is_panning_) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    QPoint dlt = event->pos() - last_pan_pos_;
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dlt.x());
    verticalScrollBar()->setValue(verticalScrollBar()->value() - dlt.y());

    last_pan_pos_ = event->pos();

    event->accept();
}

void GridView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton && is_panning_) {
        is_panning_ = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

}  // namespace komaru::editor
