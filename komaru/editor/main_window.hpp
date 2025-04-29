#pragma once
#include <QMainWindow>

namespace komaru::editor {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow() = default;
    ~MainWindow() = default;
};

}  // namespace komaru::editor
