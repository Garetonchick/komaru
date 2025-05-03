#pragma once

#include <QGraphicsView>
#include <QToolBar>

namespace komaru::editor {

class Pin;

class GridView : public QGraphicsView {
    Q_OBJECT

public:
    explicit GridView(QGraphicsScene* scene, QWidget* parent = nullptr);

    void SetLineGap(qreal gap);
    void SetBaseColor(QColor color);
    void SetBaseWidth(qreal width);
    void SetMajorWidthMul(qreal mul);
    void SetMajorDarkerFactor(int factor);
    void SetMajorMod(size_t mod);
    void SetZoomSpeed(qreal speed);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void wheelEvent(QWheelEvent* event) override;
    void scrollContentsBy(int dx, int dy) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void Zoom(qreal mul);
    void SetupToolbar();
    void PositionToolbar();

private slots:
    void OnRunAction();
    void OnSaveAction();
    void OnLoadAction();

private:
    qreal line_gap_{20.0};
    QColor base_color_{QColor::fromRgbF(100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0)};
    qreal base_width_{1.0};
    qreal major_width_mul_{1.5};
    int major_darker_factor_{120};
    size_t major_mod_{5};
    qreal zoom_speed_{1.15};
    int icon_size_{24};

    bool is_panning_{false};
    QPoint last_pan_pos_{};
    bool disable_scrolling_{false};
    QGraphicsLineItem* pending_conn_{nullptr};
    Pin* conn_start_pin_{nullptr};
    QToolBar* toolbar_{nullptr};
};

}  // namespace komaru::editor
