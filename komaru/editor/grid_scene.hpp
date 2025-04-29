#pragma once

#include <QGraphicsScene>

namespace komaru::editor {

class GridScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit GridScene(QObject* parent = nullptr);
};

}  // namespace komaru::editor
