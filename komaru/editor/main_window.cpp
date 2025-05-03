#include "main_window.hpp"

#include <komaru/editor/grid_scene.hpp>
#include <komaru/editor/grid_view.hpp>

#include <QVBoxLayout>
#include <QSplitter>
#include <QProcess>
#include <QShortcut>

namespace komaru::editor {

MainWindow::MainWindow() {
    QWidget* central_widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central_widget);
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    GridScene* scene = new GridScene(this);
    GridView* view = new GridView(scene, central_widget);

    layout->addWidget(splitter);
    layout->setContentsMargins(0, 0, 0, 0);

    splitter->addWidget(view);

    term_widget_ = new QTermWidget(0, splitter);
    term_widget_->setColorScheme("Linux");
    term_widget_->setTerminalOpacity(0.9);
    term_widget_->setScrollBarPosition(QTermWidget::ScrollBarRight);
    term_widget_->setShellProgram("/bin/bash");

    QStringList environment = QProcess::systemEnvironment();
    environment << "TERM=xterm-256color";

    term_widget_->setEnvironment(environment);
    term_widget_->startShellProgram();

    splitter->addWidget(term_widget_);

    term_widget_->hide();

    QShortcut* term_shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_H), this);
    connect(term_shortcut, &QShortcut::activated, this, &MainWindow::ToggleTerminal);

    QList<int> sizes;
    sizes << 700 << 300;
    splitter->setSizes(sizes);

    central_widget->setLayout(layout);
    setCentralWidget(central_widget);

    setWindowTitle(tr("Komaru"));
    resize(800, 600);
}

void MainWindow::ToggleTerminal() {
    if (term_widget_->isVisible()) {
        term_widget_->hide();
        return;
    }
    term_widget_->show();
    term_widget_->update();
    term_widget_->setFocus();
}

}  // namespace komaru::editor
