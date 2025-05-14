#include "main_window.hpp"

#include <komaru/editor/grid_scene.hpp>
#include <komaru/editor/grid_view.hpp>

#include <QVBoxLayout>
#include <QSplitter>
#include <QProcess>
#include <QShortcut>
#include <QDockWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QListWidget>

namespace komaru::editor {

MainWindow::MainWindow() {
    QWidget* central_widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central_widget);
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    term_widget_ = new QTermWidget(0, splitter);
    GridScene* scene = new GridScene(this);

    SetupModulesSidebar();

    GridView* view =
        new GridView(scene, term_widget_, packages_list_, imports_list_, central_widget);

    layout->addWidget(splitter);
    layout->setContentsMargins(0, 0, 0, 0);

    splitter->addWidget(view);

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

void MainWindow::ToggleSidebar() {
    if (modules_sidebar_->isVisible()) {
        modules_sidebar_->hide();
        return;
    }
    modules_sidebar_->show();
    modules_sidebar_->setFocus();
}

void MainWindow::SetupModulesSidebar() {
    modules_sidebar_ = new QDockWidget(tr("Modules"), this);
    modules_tab_widget_ = new QTabWidget(modules_sidebar_);

    packages_tab_ = CreatePackagesTab();
    imports_tab_ = CreateImportsTab();

    modules_tab_widget_->addTab(packages_tab_, tr("Packages"));
    modules_tab_widget_->addTab(imports_tab_, tr("Imports"));

    modules_sidebar_->setWidget(modules_tab_widget_);
    addDockWidget(Qt::RightDockWidgetArea, modules_sidebar_);
    modules_sidebar_->hide();

    QShortcut* toggle_sidebar_shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_G), this);
    connect(toggle_sidebar_shortcut, &QShortcut::activated, this, &MainWindow::ToggleSidebar);
}

QWidget* MainWindow::CreatePackagesTab() {
    QWidget* package_tab_widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(package_tab_widget);

    package_edit_ = new QLineEdit();
    package_edit_->setPlaceholderText(tr("Your package name"));
    layout->addWidget(package_edit_);

    packages_list_ = new QListWidget();
    layout->addWidget(packages_list_);

    connect(package_edit_, &QLineEdit::returnPressed, this, &MainWindow::AddPackage);

    QShortcut* delete_shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), packages_list_);
    connect(delete_shortcut, &QShortcut::activated, this, &MainWindow::DeletePackages);

    package_tab_widget->setLayout(layout);
    return package_tab_widget;
}

QWidget* MainWindow::CreateImportsTab() {
    QWidget* import_tab_widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(import_tab_widget);

    import_edit_ = new QLineEdit();
    import_edit_->setPlaceholderText(tr("Your import"));
    layout->addWidget(import_edit_);

    imports_list_ = new QListWidget();
    layout->addWidget(imports_list_);

    connect(import_edit_, &QLineEdit::returnPressed, this, &MainWindow::AddImport);

    QShortcut* delete_shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), imports_list_);
    connect(delete_shortcut, &QShortcut::activated, this, &MainWindow::DeleteImports);

    import_tab_widget->setLayout(layout);
    return import_tab_widget;
}

void MainWindow::AddPackage() {
    QString text = package_edit_->text().trimmed();
    if (!text.isEmpty()) {
        packages_list_->addItem(text);
        package_edit_->clear();
    }
}

void MainWindow::AddImport() {
    QString text = import_edit_->text().trimmed();
    if (!text.isEmpty()) {
        imports_list_->addItem(text);
        import_edit_->clear();
    }
}

void MainWindow::DeletePackages() {
    QList<QListWidgetItem*> selected = packages_list_->selectedItems();
    for (QListWidgetItem* item : selected) {
        delete item;
    }
}

void MainWindow::DeleteImports() {
    QList<QListWidgetItem*> selected = imports_list_->selectedItems();
    for (QListWidgetItem* item : selected) {
        delete item;
    }
}

}  // namespace komaru::editor
