#include "ThemeEditorDock.h"
#include "ThemeManager.h"
#include "ThemeLibraryManager.h"
#include "ThemeStorage.h"
#include "ThemeApplier.h"
#include "StyleParser.h"
#include "SettingsManager.h"
#include "ColorPickerWidget.h"
#include "QSSEditorWidget.h"
#include "WidgetInspector.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSplitter>

ThemeEditorDock::ThemeEditorDock(QWidget *parent)
    : QDockWidget(parent) {
    setObjectName("StylerXDock");
    setWindowTitle("StylerX Studio");
    setMinimumSize(400, 500);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
                QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
    setAllowedAreas(Qt::AllDockWidgetAreas);

    m_widgetInspector = new WidgetInspector(this);

    setupUI();
    setupActions();
    setupConnections();

    loadSettings();
}

ThemeEditorDock::~ThemeEditorDock() {
    cleanup();
    saveSettings();
}

void ThemeEditorDock::cleanup() {
    m_widgetInspector->setEnabled(false);
}

void ThemeEditorDock::setupUI() {
    QWidget *dockWidget = new QWidget(this);
    QVBoxLayout *dockLayout = new QVBoxLayout(dockWidget);
    dockLayout->setContentsMargins(0, 0, 0, 0);
    dockLayout->setSpacing(0);

    QWidget *topBar = new QWidget(this);
    topBar->setStyleSheet("background-color: #252526; border-bottom: 1px solid #3c3c3c;");
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(8, 6, 8, 6);
    topLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel("StylerX Studio", this);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ccc; padding: 2px 0;");
    topLayout->addWidget(titleLabel);

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search themes, colors, properties...");
    m_searchInput->setClearButtonEnabled(true);
    m_searchInput->setStyleSheet(
        "QLineEdit { background-color: #3c3c3c; color: #ccc; border: 1px solid #555; "
        "border-radius: 3px; padding: 4px 8px; }");
    connect(m_searchInput, &QLineEdit::textChanged, this, &ThemeEditorDock::onSearchChanged);
    topLayout->addWidget(m_searchInput);

    dockLayout->addWidget(topBar);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setStyleSheet("QSplitter::handle { background-color: #3c3c3c; width: 1px; }");

    QWidget *sidebarWidget = new QWidget(this);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(4, 4, 4, 4);
    sidebarLayout->setSpacing(4);

    QLabel *themesLabel = new QLabel("Themes", this);
    themesLabel->setStyleSheet("font-size: 11px; font-weight: bold; color: #888; padding: 2px 4px;");
    sidebarLayout->addWidget(themesLabel);

    m_themeList = new QListWidget(this);
    m_themeList->setStyleSheet(
        "QListWidget { background-color: #1e1e1e; border: 1px solid #3c3c3c; "
        "border-radius: 3px; }"
        "QListWidget::item { color: #ccc; padding: 4px 6px; border-radius: 2px; }"
        "QListWidget::item:selected { background-color: #264f78; color: #fff; }"
        "QListWidget::item:hover { background-color: #2d2d30; }");
    m_themeList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_themeList, &QListWidget::customContextMenuRequested, this, &ThemeEditorDock::onThemeContextMenu);
    connect(m_themeList, &QListWidget::itemClicked, this, &ThemeEditorDock::onThemeSelected);
    sidebarLayout->addWidget(m_themeList, 1);

    m_themeCountLabel = new QLabel("0 themes", this);
    m_themeCountLabel->setStyleSheet("font-size: 10px; color: #666; padding: 0 4px;");
    sidebarLayout->addWidget(m_themeCountLabel);

    QHBoxLayout *themeBtnLayout = new QHBoxLayout();
    themeBtnLayout->setSpacing(3);
    m_addBtn = new QPushButton("+", this);
    m_addBtn->setToolTip("New Theme");
    m_addBtn->setFixedSize(24, 24);
    m_saveBtn = new QPushButton(QChar(0x2713), this);
    m_saveBtn->setToolTip("Save Current");
    m_saveBtn->setFixedSize(24, 24);
    m_duplicateBtn = new QPushButton("D", this);
    m_duplicateBtn->setToolTip("Duplicate");
    m_duplicateBtn->setFixedSize(24, 24);
    m_deleteBtn = new QPushButton("X", this);
    m_deleteBtn->setToolTip("Delete");
    m_deleteBtn->setFixedSize(24, 24);
    for (auto *btn : {m_addBtn, m_saveBtn, m_duplicateBtn, m_deleteBtn}) {
        btn->setStyleSheet(
            "QPushButton { background-color: #3c3c3c; color: #ccc; border: 1px solid #555; "
            "border-radius: 3px; font-weight: bold; font-size: 11px; }"
            "QPushButton:hover { background-color: #505050; }");
    }
    themeBtnLayout->addWidget(m_addBtn);
    themeBtnLayout->addWidget(m_saveBtn);
    themeBtnLayout->addWidget(m_duplicateBtn);
    themeBtnLayout->addWidget(m_deleteBtn);
    themeBtnLayout->addStretch();
    sidebarLayout->addLayout(themeBtnLayout);

    QHBoxLayout *fileBtnLayout = new QHBoxLayout();
    fileBtnLayout->setSpacing(3);
    m_exportBtn = new QPushButton("Export", this);
    m_importBtn = new QPushButton("Import", this);
    m_resetBtn = new QPushButton("Reset", this);
    for (auto *btn : {m_exportBtn, m_importBtn, m_resetBtn}) {
        btn->setStyleSheet(
            "QPushButton { background-color: #2d2d30; color: #aaa; border: 1px solid #3c3c3c; "
            "border-radius: 3px; padding: 3px 8px; font-size: 10px; }"
            "QPushButton:hover { background-color: #3c3c3c; color: #ccc; }");
    }
    fileBtnLayout->addWidget(m_exportBtn);
    fileBtnLayout->addWidget(m_importBtn);
    fileBtnLayout->addWidget(m_resetBtn);
    sidebarLayout->addLayout(fileBtnLayout);

    splitter->addWidget(sidebarWidget);

    m_mainTabs = new QTabWidget(this);
    m_mainTabs->setStyleSheet(
        "QTabWidget::pane { background-color: #2d2d30; border: none; }"
        "QTabBar::tab { background-color: #252526; color: #888; padding: 6px 12px; "
        "border: none; border-right: 1px solid #3c3c3c; }"
        "QTabBar::tab:selected { background-color: #2d2d30; color: #ccc; }"
        "QTabBar::tab:hover { color: #fff; }");

    m_mainTabs->addTab(createColorTab(), "Colors");
    m_mainTabs->addTab(createQssTab(), "QSS");
    m_mainTabs->addTab(createInspectorTab(), "Inspector");

    splitter->addWidget(m_mainTabs);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setSizes({200, 600});

    dockLayout->addWidget(splitter);
    setWidget(dockWidget);
}

QWidget *ThemeEditorDock::createColorTab() {
    QWidget *tab = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_colorScroll = new QScrollArea(tab);
    m_colorScroll->setWidgetResizable(true);
    m_colorScroll->setFrameShape(QFrame::NoFrame);
    m_colorScroll->setStyleSheet(
        "QScrollArea { background-color: transparent; border: none; }"
        "QScrollBar:vertical { background-color: #1e1e1e; width: 8px; }"
        "QScrollBar::handle:vertical { background-color: #424242; border-radius: 4px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }");

    m_colorContainer = new QWidget(m_colorScroll);
    m_colorLayout = new QVBoxLayout(m_colorContainer);
    m_colorLayout->setContentsMargins(8, 8, 8, 8);
    m_colorLayout->setSpacing(2);
    m_colorLayout->addStretch();

    m_colorScroll->setWidget(m_colorContainer);
    layout->addWidget(m_colorScroll);

    populateColorPickers();
    return tab;
}

QWidget *ThemeEditorDock::createQssTab() {
    m_qssEditor = new QSSEditorWidget(this);
    connect(m_qssEditor, &QSSEditorWidget::applyRequested, this, &ThemeEditorDock::onQssApply);
    return m_qssEditor;
}

QWidget *ThemeEditorDock::createInspectorTab() {
    m_inspectorPanel = new WidgetInspectorPanel(this);
    m_inspectorPanel->setInspector(m_widgetInspector);
    connect(m_inspectorPanel, &WidgetInspectorPanel::editColorRequested, [this](const QString &) {
        m_mainTabs->setCurrentIndex(0);
    });
    return m_inspectorPanel;
}

void ThemeEditorDock::setupActions() {
    connect(m_addBtn, &QPushButton::clicked, this, &ThemeEditorDock::onAddTheme);
    connect(m_saveBtn, &QPushButton::clicked, this, &ThemeEditorDock::onSaveTheme);
    connect(m_duplicateBtn, &QPushButton::clicked, this, &ThemeEditorDock::onDuplicateTheme);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ThemeEditorDock::onDeleteTheme);
    connect(m_exportBtn, &QPushButton::clicked, this, &ThemeEditorDock::onExportTheme);
    connect(m_importBtn, &QPushButton::clicked, this, &ThemeEditorDock::onImportTheme);
    connect(m_resetBtn, &QPushButton::clicked, this, &ThemeEditorDock::onResetToDefaults);
}

void ThemeEditorDock::setupConnections() {
    auto &mgr = ThemeManager::instance();
    connect(&mgr, &ThemeManager::currentThemeChanged, this, &ThemeEditorDock::onCurrentThemeChanged);
    connect(&mgr, &ThemeManager::themeListChanged, this, &ThemeEditorDock::refreshThemeList);
}

void ThemeEditorDock::populateColorPickers() {
    for (auto *picker : m_colorPickers) {
        m_colorLayout->removeWidget(picker);
        picker->deleteLater();
    }
    m_colorPickers.clear();

    auto properties = ThemeProperties::all();
    for (const auto &prop : properties) {
        ColorPickerWidget *picker = new ColorPickerWidget(
            prop.displayName, prop.key, prop.defaultValue, this);
        connect(picker, &ColorPickerWidget::colorChanged, this, &ThemeEditorDock::onColorChanged);
        m_colorLayout->insertWidget(m_colorLayout->count() - 1, picker);
        m_colorPickers.append(picker);
    }
}

void ThemeEditorDock::refreshThemeList() {
    QString currentId = ThemeManager::instance().currentThemeId();

    m_themeList->blockSignals(true);
    m_themeList->clear();

    QString searchText = m_searchInput->text().toLower();

    auto themes = ThemeManager::instance().searchThemes(searchText);
    for (const ThemeData *theme : themes) {
        QString display = theme->name;
        if (theme->isFavorite) {
            display = QChar(0x2661) + QString(" ") + display;
        }
        QListWidgetItem *item = new QListWidgetItem(display);
        item->setData(Qt::UserRole, theme->id);
        if (theme->isFavorite) {
            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
        m_themeList->addItem(item);
        if (theme->id == currentId) {
            m_themeList->setCurrentItem(item);
        }
    }

    m_themeCountLabel->setText(QString::number(themes.size()) + " themes");
    m_themeList->blockSignals(false);
}

void ThemeEditorDock::onCurrentThemeChanged(const QString &id) {
    const ThemeData *theme = ThemeManager::instance().findTheme(id);
    if (theme) {
        applyThemeToEditors(*theme);
    }
    refreshThemeList();
}

void ThemeEditorDock::applyThemeToEditors(const ThemeData &theme) {
    m_isUpdatingPickers = true;

    QString fullStyle = StyleParser::instance().parseTheme(theme);
    m_qssEditor->setStyleSheetText(fullStyle);

    for (ColorPickerWidget *picker : m_colorPickers) {
        QColor c = theme.colors.value(picker->colorKey(), QColor("#000000"));
        picker->setColorWithoutSignal(c);
    }

    m_isUpdatingPickers = false;
}

void ThemeEditorDock::onColorChanged(const QString &key, const QColor &color) {
    if (m_isUpdatingPickers) return;

    QString currentId = ThemeManager::instance().currentThemeId();
    ThemeManager::instance().updateColor(currentId, key, color);

    const ThemeData *theme = ThemeManager::instance().currentTheme();
    if (theme) {
        QString fullStyle = StyleParser::instance().parseTheme(*theme);
        m_qssEditor->setStyleSheetText(fullStyle);
    }
}

void ThemeEditorDock::onQssApply(const QString &qss) {
    ThemeApplier::instance().applyRawStyleSheet(qss);
}

void ThemeEditorDock::onSearchChanged(const QString &text) {
    Q_UNUSED(text);
    refreshThemeList();
}

void ThemeEditorDock::onThemeSelected(QListWidgetItem *item) {
    if (!item) return;
    QString id = item->data(Qt::UserRole).toString();
    if (!id.isEmpty()) {
        ThemeManager::instance().setCurrentTheme(id);
    }
}

void ThemeEditorDock::onAddTheme() {
    ThemeData newTheme("New Theme");
    ThemeManager::instance().addTheme(newTheme);
    ThemeManager::instance().setCurrentTheme(newTheme.id);
}

void ThemeEditorDock::onSaveTheme() {
    QString currentId = ThemeManager::instance().currentThemeId();
    const ThemeData *theme = ThemeManager::instance().findTheme(currentId);
    if (theme && !theme->isReadOnly) {
        ThemeManager::instance().saveThemeToDisk(currentId);
    } else if (theme && theme->isReadOnly) {
        ThemeData copy = *theme;
        copy.id = QUuid::createUuid().toString(QUuid::Id128);
        copy.name = theme->name + " (Custom)";
        copy.isReadOnly = false;
        copy.formatVersion = ThemeData::FORMAT_VERSION;
        copy.markModified();
        ThemeManager::instance().addTheme(copy);
        ThemeManager::instance().setCurrentTheme(copy.id);
    }
}

void ThemeEditorDock::onDuplicateTheme() {
    QString currentId = ThemeManager::instance().currentThemeId();
    ThemeManager::instance().duplicateTheme(currentId);
}

void ThemeEditorDock::onDeleteTheme() {
    QString currentId = ThemeManager::instance().currentThemeId();
    const ThemeData *theme = ThemeManager::instance().findTheme(currentId);
    if (!theme) return;
    if (theme->isReadOnly) {
        QMessageBox::information(this, "Cannot Delete",
                                 "This theme is read-only. Duplicate it first to create an editable copy.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Theme",
        "Are you sure you want to delete '" + theme->name + "'?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ThemeManager::instance().removeTheme(currentId);
    }
}

void ThemeEditorDock::onExportTheme() {
    const ThemeData *theme = ThemeManager::instance().currentTheme();
    if (!theme) return;

    QString filePath = QFileDialog::getSaveFileName(
        this, "Export Theme", theme->name + ".json",
        "Theme Files (*.json);;All Files (*)");
    if (filePath.isEmpty()) return;

    QJsonObject json = ThemeStorage::themeToJson(*theme);
    QJsonDocument doc(json);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        QMessageBox::information(this, "Exported", "Theme exported successfully.");
    } else {
        QMessageBox::warning(this, "Error", "Could not write file: " + filePath);
    }
}

void ThemeEditorDock::onImportTheme() {
    QString filePath = QFileDialog::getOpenFileName(
        this, "Import Theme", "",
        "Theme Files (*.json);;All Files (*)");
    if (filePath.isEmpty()) return;

    QString error;
    ThemeData theme = ThemeLibraryManager::instance().validateAndLoadTheme(filePath, error);

    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Import Error",
                             "Could not import theme:\n\n" + error);
        return;
    }

    if (theme.colors.isEmpty()) {
        QMessageBox::warning(this, "Import Error",
                             "Could not load theme from file. The file appears to be invalid or corrupted.");
        return;
    }

    theme.id = QUuid::createUuid().toString(QUuid::Id128);
    theme.isReadOnly = false;
    theme.markModified();
    ThemeManager::instance().addTheme(theme);
    ThemeManager::instance().setCurrentTheme(theme.id);
    QMessageBox::information(this, "Imported", "Theme imported successfully.");
}

void ThemeEditorDock::onResetToDefaults() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Reset",
        "Reset to default OBS appearance? Your saved themes will not be deleted.",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ThemeManager::instance().resetToDefaults();
    }
}

void ThemeEditorDock::onThemeContextMenu(const QPoint &pos) {
    QListWidgetItem *item = m_themeList->itemAt(pos);
    if (!item) return;

    QString id = item->data(Qt::UserRole).toString();
    const ThemeData *theme = ThemeManager::instance().findTheme(id);
    if (!theme) return;

    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background-color: #2d2d30; color: #ccc; border: 1px solid #3c3c3c; }"
        "QMenu::item:selected { background-color: #264f78; }");

    QAction *renameAction = menu.addAction("Rename");
    QAction *favoriteAction = menu.addAction(theme->isFavorite ? "Unfavorite" : "Favorite");
    menu.addSeparator();
    QAction *duplicateAction = menu.addAction("Duplicate");
    QAction *exportAction = menu.addAction("Export");
    if (!theme->isReadOnly) {
        menu.addSeparator();
        QAction *deleteAction = menu.addAction("Delete");
        connect(deleteAction, &QAction::triggered, [this, id]() {
            ThemeManager::instance().removeTheme(id);
        });
    }

    QAction *selected = menu.exec(m_themeList->mapToGlobal(pos));
    if (selected == renameAction) {
        ThemeManager::instance().renameTheme(id, "Renamed Theme");
    } else if (selected == favoriteAction) {
        ThemeManager::instance().setFavorite(id, !theme->isFavorite);
    } else if (selected == duplicateAction) {
        ThemeManager::instance().duplicateTheme(id);
    } else if (selected == exportAction) {
        QJsonObject json = ThemeStorage::themeToJson(*theme);
        QString filePath = QFileDialog::getSaveFileName(
            this, "Export Theme", theme->name + ".json",
            "Theme Files (*.json);;All Files (*)");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
                file.close();
            }
        }
    }
}

void ThemeEditorDock::refreshColorPickers() {
    const ThemeData *theme = ThemeManager::instance().currentTheme();
    if (theme) {
        applyThemeToEditors(*theme);
    }
}

void ThemeEditorDock::loadSettings() {
    auto &settings = SettingsManager::instance();

    QByteArray geo = settings.dockGeometry();
    if (!geo.isEmpty()) {
        restoreGeometry(geo);
    }

    setVisible(settings.dockVisible());
}

void ThemeEditorDock::saveSettings() {
    auto &settings = SettingsManager::instance();
    settings.setDockGeometry(saveGeometry());
    settings.setDockVisible(isVisible());
}

void ThemeEditorDock::closeEvent(QCloseEvent *event) {
    saveSettings();
    emit visibilityChanged(false);
    QDockWidget::closeEvent(event);
}

void ThemeEditorDock::showEvent(QShowEvent *event) {
    QDockWidget::showEvent(event);
    emit visibilityChanged(true);
}

void ThemeEditorDock::resizeEvent(QResizeEvent *event) {
    QDockWidget::resizeEvent(event);
}
