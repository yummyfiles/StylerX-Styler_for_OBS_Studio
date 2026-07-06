#include "StyleParser.h"
#include <QRegularExpression>
#include <QTextStream>

StyleParser &StyleParser::instance() {
    static StyleParser inst;
    return inst;
}

QString StyleParser::rgb(const QColor &c) {
    return QString("rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
}

QString StyleParser::rgba(const QColor &c) {
    return QString("rgba(%1, %2, %3, %4)")
        .arg(c.red()).arg(c.green()).arg(c.blue())
        .arg(c.alphaF(), 0, 'f', 2);
}

QString StyleParser::adjustLightness(const QColor &c, qreal factor) {
    float h, s, l, a;
    c.getHslF(&h, &s, &l, &a);
    l = qBound(0.0, l * factor, 1.0);
    QColor result;
    result.setHslF(h, s, l, a);
    return result.name();
}

void StyleParser::clearCache() {
    QMutexLocker lock(&m_cacheMutex);
    m_styleCache.clear();
}

QString StyleParser::cachedStyle(const QString &themeId) const {
    QMutexLocker lock(&m_cacheMutex);
    return m_styleCache.value(themeId);
}

void StyleParser::cacheStyle(const QString &themeId, const QString &style) {
    QMutexLocker lock(&m_cacheMutex);
    m_styleCache[themeId] = style;
}

QString StyleParser::parseTheme(const ThemeData &theme) {
    QString cached = cachedStyle(theme.id);
    if (!cached.isEmpty()) {
        return cached;
    }
    QString qss = buildQSS(theme);
    cacheStyle(theme.id, qss);
    return qss;
}

QString StyleParser::updateColor(const QString &existingStyle, const QString &colorKey, const QColor &newColor) {
    static QHash<QString, QString> colorVarMap = {
        {"background",      "--theme-background"},
        {"panels",          "--theme-panels"},
        {"dock_background", "--theme-dock-bg"},
        {"buttons",         "--theme-buttons"},
        {"button_hover",    "--theme-button-hover"},
        {"button_pressed",  "--theme-button-pressed"},
        {"borders",         "--theme-borders"},
        {"accent",          "--theme-accent"},
        {"selected",        "--theme-selected"},
        {"text",            "--theme-text"},
        {"disabled_text",   "--theme-disabled-text"},
        {"menu_background", "--theme-menu-bg"},
        {"menu_text",       "--theme-menu-text"},
        {"tooltips",        "--theme-tooltips"},
        {"scrollbars",      "--theme-scrollbars"},
        {"sliders",         "--theme-sliders"},
        {"checkboxes",      "--theme-checkboxes"},
        {"input_fields",    "--theme-input-fields"},
        {"tabs",            "--theme-tabs"},
        {"status_bar",      "--theme-status-bar"},
    };

    QString var = colorVarMap.value(colorKey);
    if (var.isEmpty()) return existingStyle;

    QString newHex = newColor.name(QColor::HexRgb);
    QString regexStr = QRegularExpression::escape(var) + "\\s*:\\s*[^;]+;";
    QRegularExpression re(regexStr);
    QString replacement = var + ": " + newHex + ";";

    QString result = existingStyle;
    result.replace(re, replacement);
    return result;
}

QString StyleParser::buildQSS(const ThemeData &theme) const {
    QString qss;
    QTextStream s(&qss);

    const auto &c = theme.colors;
    auto get = [&](const QString &key, const QColor &fallback = Qt::black) -> QColor {
        return c.value(key, fallback);
    };

    QColor bg        = get("background",        QColor("#1e1e1e"));
    QColor panels    = get("panels",            QColor("#252526"));
    QColor dockBg    = get("dock_background",   QColor("#2d2d30"));
    QColor btn       = get("buttons",           QColor("#3c3c3c"));
    QColor btnHov    = get("button_hover",      QColor("#505050"));
    QColor btnPrs    = get("button_pressed",    QColor("#2d2d2d"));
    QColor border    = get("borders",           QColor("#3c3c3c"));
    QColor accent    = get("accent",            QColor("#0078d4"));
    QColor sel       = get("selected",          QColor("#264f78"));
    QColor txt       = get("text",              QColor("#cccccc"));
    QColor disTxt    = get("disabled_text",     QColor("#6c6c6c"));
    QColor menuBg    = get("menu_background",   QColor("#2d2d30"));
    QColor menuTxt   = get("menu_text",         QColor("#cccccc"));
    QColor tip       = get("tooltips",          QColor("#3c3c3c"));
    QColor scroll    = get("scrollbars",        QColor("#424242"));
    QColor slider    = get("sliders",           QColor("#424242"));
    QColor chk       = get("checkboxes",        QColor("#3c3c3c"));
    QColor input     = get("input_fields",      QColor("#3c3c3c"));
    QColor tab       = get("tabs",              QColor("#2d2d30"));
    QColor status    = get("status_bar",        QColor("#007acc"));

    QString bgStr      = rgb(bg);
    QString panelsStr  = rgb(panels);
    QString dockBgStr  = rgb(dockBg);
    QString btnStr     = rgb(btn);
    QString btnHovStr  = rgb(btnHov);
    QString btnPrsStr  = rgb(btnPrs);
    QString borderStr  = rgb(border);
    QString accentStr  = rgb(accent);
    QString selStr     = rgb(sel);
    QString txtStr     = rgb(txt);
    QString disTxtStr  = rgb(disTxt);
    QString menuBgStr  = rgb(menuBg);
    QString menuTxtStr = rgb(menuTxt);
    QString tipStr     = rgb(tip);
    QString scrollStr  = rgb(scroll);
    QString sliderStr  = rgb(slider);
    QString chkStr     = rgb(chk);
    QString inputStr   = rgb(input);
    QString tabStr     = rgb(tab);
    QString statusStr  = rgb(status);

    QString bgHex      = bg.name();
    QString panelsHex  = panels.name();
    QString dockBgHex  = dockBg.name();
    QString btnHex     = btn.name();
    QString borderHex  = border.name();
    QString accentHex  = accent.name();

    s << "/* StylerX - Auto-generated stylesheet */\n\n";

    s << "/* Root variables for dynamic updates */\n";
    s << "QMainWindow, QDialog, QWidget#centralWidget {\n";
    s << "  background-color: " << bgStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "}\n\n";

    s << "/* Panels & Docks */\n";
    s << "QWidget#panels, QWidget#panelArea, QWidget#panelContainer,\n";
    s << "QDockWidget, QDockWidget::title {\n";
    s << "  background-color: " << dockBgStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "}\n\n";

    s << "QDockWidget::close-button, QDockWidget::float-button {\n";
    s << "  background-color: " << btnStr << ";\n";
    s << "}\n";
    s << "QDockWidget::close-button:hover, QDockWidget::float-button:hover {\n";
    s << "  background-color: " << btnHovStr << ";\n";
    s << "}\n\n";

    s << "/* Buttons */\n";
    s << "QPushButton, QToolButton {\n";
    s << "  background-color: " << btnStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  padding: 4px 12px;\n";
    s << "  border-radius: 3px;\n";
    s << "}\n";
    s << "QPushButton:hover, QToolButton:hover {\n";
    s << "  background-color: " << btnHovStr << ";\n";
    s << "  border-color: " << accentStr << ";\n";
    s << "}\n";
    s << "QPushButton:pressed, QToolButton:pressed {\n";
    s << "  background-color: " << btnPrsStr << ";\n";
    s << "}\n";
    s << "QPushButton:disabled, QToolButton:disabled {\n";
    s << "  background-color: " << panelsStr << ";\n";
    s << "  color: " << disTxtStr << ";\n";
    s << "}\n\n";

    s << "/* Input Fields */\n";
    s << "QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {\n";
    s << "  background-color: " << inputStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-radius: 3px;\n";
    s << "  padding: 2px 4px;\n";
    s << "}\n";
    s << "QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {\n";
    s << "  border-color: " << accentStr << ";\n";
    s << "}\n";
    s << "QComboBox::drop-down {\n";
    s << "  background-color: " << btnStr << ";\n";
    s << "  border: none;\n";
    s << "}\n";
    s << "QComboBox::down-arrow {\n";
    s << "  image: none;\n";
    s << "}\n";
    s << "QComboBox QAbstractItemView {\n";
    s << "  background-color: " << menuBgStr << ";\n";
    s << "  color: " << menuTxtStr << ";\n";
    s << "  selection-background-color: " << accentStr << ";\n";
    s << "  selection-color: " << txtStr << ";\n";
    s << "}\n\n";

    s << "/* Scrollbars */\n";
    s << "QScrollBar:vertical {\n";
    s << "  background-color: " << panelsStr << ";\n";
    s << "  width: 12px;\n";
    s << "  margin: 0;\n";
    s << "}\n";
    s << "QScrollBar::handle:vertical {\n";
    s << "  background-color: " << scrollStr << ";\n";
    s << "  min-height: 20px;\n";
    s << "  border-radius: 3px;\n";
    s << "  margin: 2px;\n";
    s << "}\n";
    s << "QScrollBar::handle:vertical:hover {\n";
    s << "  background-color: " << rgb(adjustLightness(scroll, 1.2)) << ";\n";
    s << "}\n";
    s << "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n";
    s << "  height: 0;\n";
    s << "}\n";
    s << "QScrollBar:horizontal {\n";
    s << "  background-color: " << panelsStr << ";\n";
    s << "  height: 12px;\n";
    s << "}\n";
    s << "QScrollBar::handle:horizontal {\n";
    s << "  background-color: " << scrollStr << ";\n";
    s << "  min-width: 20px;\n";
    s << "  border-radius: 3px;\n";
    s << "  margin: 2px;\n";
    s << "}\n";
    s << "QScrollBar::handle:horizontal:hover {\n";
    s << "  background-color: " << rgb(adjustLightness(scroll, 1.2)) << ";\n";
    s << "}\n";
    s << "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {\n";
    s << "  width: 0;\n";
    s << "}\n\n";

    s << "/* Sliders */\n";
    s << "QSlider::groove:horizontal {\n";
    s << "  background-color: " << rgb(adjustLightness(slider, 0.7)) << ";\n";
    s << "  height: 6px;\n";
    s << "  border-radius: 3px;\n";
    s << "}\n";
    s << "QSlider::handle:horizontal {\n";
    s << "  background-color: " << sliderStr << ";\n";
    s << "  width: 16px;\n";
    s << "  height: 16px;\n";
    s << "  margin: -5px 0;\n";
    s << "  border-radius: 8px;\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "}\n";
    s << "QSlider::handle:horizontal:hover {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "}\n";
    s << "QSlider::sub-page:horizontal {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "  border-radius: 3px;\n";
    s << "}\n\n";

    s << "/* Checkboxes & Radio Buttons */\n";
    s << "QCheckBox, QRadioButton {\n";
    s << "  color: " << txtStr << ";\n";
    s << "  spacing: 6px;\n";
    s << "}\n";
    s << "QCheckBox::indicator {\n";
    s << "  width: 14px;\n";
    s << "  height: 14px;\n";
    s << "  background-color: " << chkStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-radius: 2px;\n";
    s << "}\n";
    s << "QCheckBox::indicator:checked {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "  border-color: " << accentStr << ";\n";
    s << "}\n";
    s << "QCheckBox::indicator:hover {\n";
    s << "  border-color: " << accentStr << ";\n";
    s << "}\n";
    s << "QRadioButton::indicator {\n";
    s << "  width: 14px;\n";
    s << "  height: 14px;\n";
    s << "  background-color: " << chkStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-radius: 7px;\n";
    s << "}\n";
    s << "QRadioButton::indicator:checked {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "  border-color: " << accentStr << ";\n";
    s << "}\n\n";

    s << "/* Tabs */\n";
    s << "QTabWidget::pane {\n";
    s << "  background-color: " << dockBgStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-top: none;\n";
    s << "}\n";
    s << "QTabBar::tab {\n";
    s << "  background-color: " << rgb(adjustLightness(tab, 0.85)) << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  padding: 6px 16px;\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-bottom: none;\n";
    s << "  border-top-left-radius: 3px;\n";
    s << "  border-top-right-radius: 3px;\n";
    s << "  margin-right: 2px;\n";
    s << "}\n";
    s << "QTabBar::tab:selected {\n";
    s << "  background-color: " << dockBgStr << ";\n";
    s << "  border-bottom-color: " << dockBgStr << ";\n";
    s << "}\n";
    s << "QTabBar::tab:hover:!selected {\n";
    s << "  background-color: " << btnStr << ";\n";
    s << "}\n\n";

    s << "/* Menus */\n";
    s << "QMenuBar {\n";
    s << "  background-color: " << menuBgStr << ";\n";
    s << "  color: " << menuTxtStr << ";\n";
    s << "  border-bottom: 1px solid " << borderStr << ";\n";
    s << "}\n";
    s << "QMenuBar::item:selected {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "}\n";
    s << "QMenu {\n";
    s << "  background-color: " << menuBgStr << ";\n";
    s << "  color: " << menuTxtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "}\n";
    s << "QMenu::item:selected {\n";
    s << "  background-color: " << selStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "}\n";
    s << "QMenu::item:disabled {\n";
    s << "  color: " << disTxtStr << ";\n";
    s << "}\n";
    s << "QMenu::separator {\n";
    s << "  height: 1px;\n";
    s << "  background-color: " << borderStr << ";\n";
    s << "  margin: 4px 8px;\n";
    s << "}\n\n";

    s << "/* Tooltips */\n";
    s << "QToolTip {\n";
    s << "  background-color: " << tipStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  padding: 4px;\n";
    s << "  border-radius: 3px;\n";
    s << "}\n\n";

    s << "/* Status Bar */\n";
    s << "QStatusBar {\n";
    s << "  background-color: " << statusStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border-top: 1px solid " << borderStr << ";\n";
    s << "}\n";
    s << "QStatusBar::item {\n";
    s << "  border: none;\n";
    s << "}\n\n";

    s << "/* Groups */\n";
    s << "QGroupBox {\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-radius: 3px;\n";
    s << "  margin-top: 8px;\n";
    s << "  padding-top: 8px;\n";
    s << "}\n";
    s << "QGroupBox::title {\n";
    s << "  color: " << txtStr << ";\n";
    s << "  subcontrol-origin: margin;\n";
    s << "  left: 10px;\n";
    s << "  padding: 0 4px;\n";
    s << "}\n\n";

    s << "/* List & Tree Widgets */\n";
    s << "QListWidget, QTreeWidget, QTableWidget, QListView, QTreeView, QTableView {\n";
    s << "  background-color: " << panelsStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  alternate-background-color: " << bgStr << ";\n";
    s << "}\n";
    s << "QListWidget::item:selected, QTreeWidget::item:selected,\n";
    s << "QTableWidget::item:selected, QListView::item:selected,\n";
    s << "QTreeView::item:selected, QTableView::item:selected {\n";
    s << "  background-color: " << selStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "}\n";
    s << "QListWidget::item:hover, QTreeWidget::item:hover,\n";
    s << "QTableWidget::item:hover, QListView::item:hover,\n";
    s << "QTreeView::item:hover, QTableView::item:hover {\n";
    s << "  background-color: " << rgb(adjustLightness(sel, 1.5)) << ";\n";
    s << "}\n\n";

    s << "/* Headers */\n";
    s << "QHeaderView::section {\n";
    s << "  background-color: " << btnStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  padding: 4px;\n";
    s << "}\n";
    s << "QHeaderView::section:hover {\n";
    s << "  background-color: " << btnHovStr << ";\n";
    s << "}\n\n";

    s << "/* Progress Bar */\n";
    s << "QProgressBar {\n";
    s << "  background-color: " << panelsStr << ";\n";
    s << "  color: " << txtStr << ";\n";
    s << "  border: 1px solid " << borderStr << ";\n";
    s << "  border-radius: 3px;\n";
    s << "  text-align: center;\n";
    s << "}\n";
    s << "QProgressBar::chunk {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "  border-radius: 2px;\n";
    s << "}\n\n";

    s << "/* Splitter */\n";
    s << "QSplitter::handle {\n";
    s << "  background-color: " << borderStr << ";\n";
    s << "}\n";
    s << "QSplitter::handle:horizontal { width: 2px; }\n";
    s << "QSplitter::handle:vertical { height: 2px; }\n";
    s << "QSplitter::handle:hover {\n";
    s << "  background-color: " << accentStr << ";\n";
    s << "}\n\n";

    s << "/* Labels */\n";
    s << "QLabel {\n";
    s << "  color: " << txtStr << ";\n";
    s << "  background-color: transparent;\n";
    s << "}\n\n";

    s << "/* Toolbar */\n";
    s << "QToolBar {\n";
    s << "  background-color: " << menuBgStr << ";\n";
    s << "  border-bottom: 1px solid " << borderStr << ";\n";
    s << "  spacing: 4px;\n";
    s << "}\n";
    s << "QToolBar::separator {\n";
    s << "  background-color: " << borderStr << ";\n";
    s << "  width: 1px;\n";
    s << "  margin: 4px 2px;\n";
    s << "}\n\n";

    s << "/* Scroll areas */\n";
    s << "QScrollArea {\n";
    s << "  background-color: transparent;\n";
    s << "  border: none;\n";
    s << "}\n\n";

    return qss;
}
