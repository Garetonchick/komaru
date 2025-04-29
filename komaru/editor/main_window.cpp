#include "main_window.hpp"

#include <komaru/editor/grid_scene.hpp>
#include <komaru/editor/grid_view.hpp>

namespace komaru::editor {

MainWindow::MainWindow() {
    GridScene* scene = new GridScene(this);
    GridView* view = new GridView(scene, this);
    setCentralWidget(view);
}

}  // namespace komaru::editor
