#pragma once

#include <QMainWindow>
#include <qtermwidget6/qtermwidget.h>

class QLineEdit;
class QListWidget;

namespace komaru::editor {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void ToggleTerminal();
    void ToggleSidebar();

private:
    void SetupModulesSidebar();
    QWidget* CreatePackagesTab();
    QWidget* CreateImportsTab();
    void AddPackage();
    void AddImport();
    void DeletePackages();
    void DeleteImports();

private:
    QTermWidget* term_widget_;

    QDockWidget* modules_sidebar_;
    QTabWidget* modules_tab_widget_;
    QWidget* packages_tab_;
    QLineEdit* package_edit_;
    QListWidget* packages_list_;
    QWidget* imports_tab_;
    QLineEdit* import_edit_;
    QListWidget* imports_list_;
};

}  // namespace komaru::editor
