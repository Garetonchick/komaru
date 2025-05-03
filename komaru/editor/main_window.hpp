#pragma once

#include <QMainWindow>
#include <qtermwidget6/qtermwidget.h>

namespace komaru::editor {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void ToggleTerminal();

private:
    QTermWidget* term_widget_;
};

}  // namespace komaru::editor
