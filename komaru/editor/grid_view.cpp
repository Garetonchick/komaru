#include "grid_view.hpp"

#include <komaru/editor/node.hpp>
#include <komaru/editor/connection.hpp>
#include <komaru/translate/cat_cooking.hpp>
#include <komaru/translate/haskell/hs_translator.hpp>
#include <komaru/translate/haskell/hs_symbols_registry.hpp>
#include <komaru/translate/exec_program.hpp>
#include <komaru/util/std_extensions.hpp>
#include <komaru/util/filesystem.hpp>

#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QPushButton>
#include <QString>
#include <QStyle>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QListWidget>
#include <qtermwidget6/qtermwidget.h>

namespace komaru::editor {

std::string ConvString(const QString& qs) {
    std::string s(qs.size(), ' ');

    for (int64_t i = 0; i < qs.size(); ++i) {
        QChar c = *(qs.data() + i);
        s[i] = c.toLatin1();
    }
    return s;
}

GridView::GridView(QGraphicsScene* scene, QTermWidget* terminal, QListWidget* packages_list,
                   QListWidget* imports_list, QWidget* parent)
    : QGraphicsView(scene, parent),
      terminal_(terminal),
      packages_list_(packages_list),
      imports_list_(imports_list) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setSceneRect(-1'000'000, -1'000'000, 2'000'000, 2'000'000);
    setMouseTracking(true);

    SetupToolbar();
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
        bool is_major = (std::lround(std::abs(pos) / line_gap_) % major_mod_) == 0;
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
    if (is_panning_) {
        event->accept();
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

void GridView::SetupToolbar() {
    toolbar_ = new QToolBar(this);
    toolbar_->setMovable(false);
    toolbar_->setFloatable(false);
    toolbar_->setStyleSheet(
        "QToolBar { background-color: rgba(240, 240, 240, 220); border-radius: 4px; border: 1px "
        "solid darkgray; }");

    auto* run_button = new QPushButton(toolbar_);
    auto* save_button = new QPushButton(toolbar_);
    auto* load_button = new QPushButton(toolbar_);

    run_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    save_button->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    load_button->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));

    run_button->setToolTip("Run");
    save_button->setToolTip("Save");
    load_button->setToolTip("Load");

    QString button_style =
        "QPushButton { min-width: 40px; min-height: 40px; padding: 5px; margin: 2px; "
        "background-color: #f0f0f0; border: 1px solid #c0c0c0; border-radius: 3px; }"
        "QPushButton:hover { background-color: #e0e0e0; }"
        "QPushButton:pressed { background-color: #d0d0d0; }";

    run_button->setStyleSheet(button_style);
    save_button->setStyleSheet(button_style);
    load_button->setStyleSheet(button_style);

    run_button->setIconSize(QSize(icon_size_, icon_size_));
    save_button->setIconSize(QSize(icon_size_, icon_size_));
    load_button->setIconSize(QSize(icon_size_, icon_size_));

    toolbar_->addWidget(run_button);
    toolbar_->addWidget(save_button);
    toolbar_->addWidget(load_button);

    connect(run_button, &QPushButton::clicked, this, &GridView::OnRunAction);
    connect(save_button, &QPushButton::clicked, this, &GridView::OnSaveAction);
    connect(load_button, &QPushButton::clicked, this, &GridView::OnLoadAction);

    PositionToolbar();
}

void GridView::PositionToolbar() {
    int x = (width() - toolbar_->sizeHint().width()) / 2;
    toolbar_->move(x, 10);
    toolbar_->raise();
}

translate::RawCatProgram GridView::ConvertNodeGraphToRawCatProgram() const {
    translate::RawCatProgram prog;
    std::unordered_map<const Node*, size_t> node2id;

    for (Node* node : nodes_) {
        bool has_input = node->GetInputPin() != nullptr;
        std::optional<std::string> name;
        std::string type = ConvString(node->GetMainText()->toPlainText());
        const Text* tag_text = node->GetTagText();

        if (tag_text && !tag_text->toPlainText().isEmpty()) {
            name = ConvString(tag_text->toPlainText());
        }

        std::vector<std::string> branchers;

        auto& out_pins = node->GetOutputPins();

        if (out_pins.size() == 1) {
            auto* label = node->GetPinLabel(out_pins.front());
            if (label) {
                branchers.push_back(ConvString(label->toPlainText()));
            } else {
                branchers.push_back("*");
            }
        } else {
            for (Pin* out_pin : out_pins) {
                branchers.push_back(ConvString(node->GetPinLabel(out_pin)->toPlainText()));
            }
        }

        size_t id = prog.NewNodeGeneric(type, name, branchers, has_input);
        node2id.emplace(node, id);
    }

    for (Node* node : nodes_) {
        auto& out_pins = node->GetOutputPins();
        size_t source_id = node2id[node];

        for (const auto [i, out_pin] : util::Enumerate(out_pins)) {
            const std::vector<Connection*>& conns = out_pin->GetConnections();

            for (const auto& conn : conns) {
                size_t target_id = node2id[conn->GetTargetPin()->GetNode()];
                std::string morphism = ConvString(conn->GetText()->toPlainText());

                prog.Connect(source_id, target_id, i, morphism);
            }
        }
    }

    return prog;
}

QJsonDocument GridView::ToJson() {
    QJsonDocument json;
    QJsonArray json_nodes;
    std::unordered_map<const Node*, qint64> node2id;

    for (auto [i, node] : util::Enumerate(nodes_)) {
        node2id[node] = i;
    }

    for (Node* node : nodes_) {
        QJsonObject json_node;
        json_node.insert("id", node2id[node]);
        json_node.insert("x", node->pos().x());
        json_node.insert("y", node->pos().y());
        json_node.insert("main_text", node->GetMainText()->toPlainText());
        const Text* tag_text = node->GetTagText();

        if (tag_text) {
            json_node.insert("tag_text", tag_text->toPlainText());
        }

        json_node.insert("has_input_pin", node->GetInputPin() != nullptr);

        QJsonArray json_pins;

        for (Pin* out_pin : node->GetOutputPins()) {
            QJsonObject json_pin;
            const Text* pin_label = node->GetPinLabel(out_pin);
            if (pin_label) {
                json_pin.insert("label", pin_label->toPlainText());
            }
            QJsonArray json_conns;

            for (Connection* conn : out_pin->GetConnections()) {
                const Node* target_node = conn->GetTargetPin()->GetNode();
                qint64 target_node_id = node2id[target_node];
                QJsonObject json_conn;

                json_conn.insert("text", conn->GetText()->toPlainText());
                json_conn.insert("node", target_node_id);

                json_conns.append(json_conn);
            }

            json_pin.insert("connections", json_conns);

            json_pins.append(json_pin);
        }

        json_node.insert("out_pins", json_pins);

        json_nodes.append(json_node);
    }

    QJsonArray json_package_list;
    QJsonArray json_import_list;

    for (const auto& package : GetPackages()) {
        json_package_list.append(QString::fromStdString(package));
    }

    for (const auto& import : GetImports()) {
        json_import_list.append(QString::fromStdString(import.ToString()));
    }

    QJsonObject main_json_obj;
    main_json_obj.insert("nodes", json_nodes);
    main_json_obj.insert("packages", json_package_list);
    main_json_obj.insert("imports", json_import_list);

    json.setObject(main_json_obj);

    return json;
}

void GridView::FromJson(const QJsonDocument& json) {
    for (Node* node : nodes_) {
        scene()->removeItem(node);
        delete node;
    }

    nodes_.clear();
    packages_list_->clear();
    imports_list_->clear();

    if (!json.isObject()) {
        std::println("error parsing json: expected json object as root item");
        return;
    }

    QJsonObject main_json_obj = json.object();
    QJsonArray json_packages_list = main_json_obj["packages"].toArray();
    QJsonArray json_imports_list = main_json_obj["imports"].toArray();

    for (auto json_item : json_packages_list) {
        packages_list_->addItem(json_item.toString());
    }
    for (auto json_item : json_imports_list) {
        imports_list_->addItem(json_item.toString());
    }

    struct ConnectionInfo {
        Pin* source_pin;
        qint64 target_node_id;
        QString text;
    };

    QJsonArray json_nodes = main_json_obj["nodes"].toArray();
    std::vector<Node*> id2node(json_nodes.size(), nullptr);
    std::vector<ConnectionInfo> conn_infos;

    for (auto json_node_ref : json_nodes) {
        if (!json_node_ref.isObject()) {
            std::println("error parsing json expected node to be an object");
            return;
        }

        QJsonObject json_node = json_node_ref.toObject();

        qint64 node_id = json_node["id"].toInteger();
        qreal x = json_node["x"].toDouble();
        qreal y = json_node["y"].toDouble();
        QString main_text = json_node["main_text"].toString();
        bool has_input_pin = json_node["has_input_pin"].toBool();

        auto unode = std::make_unique<Node>();
        Node* node = unode.get();
        id2node[node_id] = node;
        nodes_.emplace(node);
        node->setPos(x, y);
        scene()->addItem(unode.release());

        node->SetMainText(main_text);

        if (json_node.contains("tag_text")) {
            node->SetTagText(json_node["tag_text"].toString());
        }

        if (!has_input_pin) {
            node->RemoveInputPin();
        }

        QJsonArray json_out_pins = json_node["out_pins"].toArray();

        if (json_out_pins.empty()) {
            node->RemoveOutputPin();
        }

        for (qint64 i = 1; i < json_out_pins.size(); ++i) {
            node->AddOutputPin();
        }

        auto& out_pins = node->GetOutputPins();

        for (auto [i, json_out_pin_ref] : util::Enumerate(json_out_pins)) {
            QJsonObject json_out_pin = json_out_pin_ref.toObject();

            if (json_out_pin.contains("label")) {
                node->SetPinLabel(out_pins[i], json_out_pin["label"].toString());
            }

            QJsonArray json_connections = json_out_pin["connections"].toArray();

            for (auto json_conn_ref : json_connections) {
                QJsonObject json_conn = json_conn_ref.toObject();
                conn_infos.emplace_back(
                    ConnectionInfo{.source_pin = out_pins[i],
                                   .target_node_id = json_conn["node"].toInteger(),
                                   .text = json_conn["text"].toString()});
            }
        }
    }

    for (auto& conn_info : conn_infos) {
        Node* target_node = id2node[conn_info.target_node_id];
        assert(target_node->GetInputPin() != nullptr);
        Connection* conn = new Connection(conn_info.source_pin, target_node->GetInputPin());
        scene()->addItem(conn);
        conn->SetText(conn_info.text);
    }
}

std::vector<std::string> GridView::GetPackages() {
    std::vector<std::string> packages;
    for (int i = 0; i < packages_list_->count(); ++i) {
        auto* item = packages_list_->item(i);

        packages.emplace_back(ConvString(item->text()));
    }

    return packages;
}

std::vector<translate::hs::HaskellImport> GridView::GetImports() {
    std::vector<translate::hs::HaskellImport> imports;

    for (int i = 0; i < imports_list_->count(); ++i) {
        auto* item = imports_list_->item(i);
        std::string text = ConvString(item->text());

        auto maybe_import = translate::hs::ParseHaskellImport(text);
        if (!maybe_import) {
            throw std::runtime_error(std::format("failed to parse import \"{}\"", text));
        }
        imports.emplace_back(maybe_import.value());
    }

    return imports;
}

void GridView::OnRunAction() {
    auto raw_program = ConvertNodeGraphToRawCatProgram();
    std::vector<std::string> packages;
    std::vector<translate::hs::HaskellImport> imports;

    try {
        packages = GetPackages();
        imports = GetImports();
    } catch (std::exception& e) {
        std::println("modules error: {}", e.what());
        return;
    }

    std::print("Packages:");
    for (auto& package : packages) {
        std::print(" \"{}\",", package);
    }
    std::println("");

    std::print("Imports:");
    for (auto& import : imports) {
        std::print(" \"{}\",", import.ToString());
    }
    std::println("");

    try {
        translate::hs::HaskellSymbolsRegistry symbols_registry(packages, imports);
        auto maybe_cat_program = translate::Cook(raw_program, symbols_registry);

        if (!maybe_cat_program) {
            std::println("cooking error: {}", maybe_cat_program.error().Error());
            return;
        }

        auto cat_program = std::move(maybe_cat_program.value());
        auto translator = translate::hs::HaskellTranslator(packages, imports);
        auto maybe_program = translator.Translate(cat_program);

        if (!maybe_program) {
            std::println("translation error: {}", maybe_program.error().Error());
            return;
        }

        auto& program = maybe_program.value();

        util::WriteFile("gen.hs", program->GetSourceCode());

        auto build_res = translate::BuildProgram(*program);

        if (build_res.command_res.Fail()) {
            std::println("build error: STDOUT: {}\nSTDERR: {}\n", build_res.command_res.Stdout(),
                         build_res.command_res.Stderr());
            return;
        }

        terminal_->sendText(QString::fromStdString(build_res.program_path + "\n"));
        terminal_->show();
        terminal_->update();
        terminal_->setFocus();
    } catch (std::exception& e) {
        std::println("Got exception: {}", e.what());
    }
}

void GridView::OnSaveAction() {
    QString file_path = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                     tr("All Files (*);;Text Files (*.txt)"));

    if (file_path.isEmpty()) {
        return;
    }

    QJsonDocument json = ToJson();
    QFile file(file_path);

    if (!file.open(QIODevice::WriteOnly)) {
        std::println("Failed to open file \"{}\" for writing", ConvString(file_path));
        return;
    }

    file.write(json.toJson());
    file.close();
}

void GridView::OnLoadAction() {
    QString file_path = QFileDialog::getOpenFileName(this, tr("Load File"), "",
                                                     tr("All Files (*);;Text Files (*.txt)"));

    if (file_path.isEmpty()) {
        return;
    }

    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly)) {
        std::println("Failed to open file \"{}\" for reading", ConvString(file_path));
        return;
    }

    auto json = QJsonDocument::fromJson(file.readAll());
    file.close();

    FromJson(json);
}

void GridView::mousePressEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());

    if (event->button() == Qt::RightButton) {
        is_panning_ = true;
        last_pan_pos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);

        event->accept();
        return;
    } else if (event->button() == Qt::LeftButton && item) {
        if (Pin* pin = dynamic_cast<Pin*>(item)) {
            QPointF pin_pos = pin->mapToScene(0, 0);
            QPointF cursor_pos = mapToScene(event->pos());
            conn_start_pin_ = pin;

            pending_conn_ = new QGraphicsLineItem();
            pending_conn_->setPen(QPen(Qt::yellow, 2));
            pending_conn_->setLine(QLineF(pin_pos, cursor_pos));
            pending_conn_->setZValue(1000);

            scene()->addItem(pending_conn_);
            setCursor(Qt::CrossCursor);
            event->accept();
            return;
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GridView::mouseMoveEvent(QMouseEvent* event) {
    if (is_panning_) {
        QPoint dlt = event->pos() - last_pan_pos_;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dlt.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - dlt.y());

        last_pan_pos_ = event->pos();

        event->accept();
    } else if (pending_conn_) {
        QPointF cursor_pos = mapToScene(event->pos());
        QPointF pin_pos = conn_start_pin_->mapToScene(0, 0);
        pending_conn_->setLine(QLineF(pin_pos, cursor_pos));
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GridView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton && is_panning_) {
        is_panning_ = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton && pending_conn_) {
        setCursor(Qt::ArrowCursor);

        scene()->removeItem(pending_conn_);
        delete pending_conn_;
        pending_conn_ = nullptr;

        QGraphicsItem* item = itemAt(event->pos());
        Pin* end_pin = dynamic_cast<Pin*>(item);

        if (end_pin && conn_start_pin_->ConnectableTo(end_pin)) {
            Pin* output_pin = conn_start_pin_;
            Pin* input_pin = end_pin;

            if (output_pin->GetPinType() != Pin::Output) {
                std::swap(output_pin, input_pin);
            }

            Connection* conn = new Connection(output_pin, input_pin);
            scene()->addItem(conn);
        }

        conn_start_pin_ = nullptr;
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void GridView::keyPressEvent(QKeyEvent* event) {
    bool ctrl = event->modifiers() & Qt::ControlModifier;
    if (event->key() == Qt::Key_A && ctrl) {
        QPointF cursor_pos = mapToScene(mapFromGlobal(QCursor::pos()));

        auto node = std::make_unique<Node>();
        nodes_.emplace(node.get());
        node->setPos(cursor_pos);
        scene()->addItem(node.release());
        event->accept();
    } else if (event->key() == Qt::Key_D && ctrl) {
        QList<QGraphicsItem*> selected = scene()->selectedItems();

        for (QGraphicsItem* item : selected) {
            if (Node* node = dynamic_cast<Node*>(item)) {
                scene()->removeItem(node);
                nodes_.erase(node);
                delete node;
            } else if (Connection* conn = dynamic_cast<Connection*>(item)) {
                conn->Detach();
                scene()->removeItem(conn);
                delete conn;
            }
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GridView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    PositionToolbar();
}

}  // namespace komaru::editor
