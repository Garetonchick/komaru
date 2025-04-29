#include <komaru/editor/main_window.hpp>

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    komaru::editor::MainWindow main_window;
    main_window.show();
    return app.exec();
}
