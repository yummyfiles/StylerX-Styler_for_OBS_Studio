#pragma once

#include <QDockWidget>
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QToolBar>
#include <QLabel>
#include <QComboBox>
#include <QMenu>
#include <QAction>
#include <QVector>
#include <QPointer>

class ColorPickerWidget;
class QSSEditorWidget;
class WidgetInspectorPanel;
class WidgetInspector;
struct ThemeData;

class ThemeEditorDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ThemeEditorDock(QWidget *parent = nullptr);
    ~ThemeEditorDock() override;

    void loadSettings();
    void saveSettings();

signals:
    void visibilityChanged(bool visible);

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onCurrentThemeChanged(const QString &id);
    void onColorChanged(const QString &key, const QColor &color);
    void onQssApply(const QString &qss);
    void onSearchChanged(const QString &text);
    void onThemeSelected(QListWidgetItem *item);
    void onAddTheme();
    void onSaveTheme();
    void onDuplicateTheme();
    void onDeleteTheme();
    void onExportTheme();
    void onImportTheme();
    void onResetToDefaults();
    void onThemeContextMenu(const QPoint &pos);
    void refreshThemeList();
    void refreshColorPickers();

private:
    void setupUI();
    void setupActions();
    void setupConnections();
    void populateColorPickers();
    void updateCurrentThemeColors();
    void applyThemeToEditors(const ThemeData &theme);
    QWidget *createColorTab();
    QWidget *createQssTab();
    QWidget *createInspectorTab();

    QTabWidget *m_mainTabs;

    QScrollArea *m_colorScroll;
    QWidget *m_colorContainer;
    QVBoxLayout *m_colorLayout;
    QVector<ColorPickerWidget*> m_colorPickers;

    QSSEditorWidget *m_qssEditor;

    WidgetInspector *m_widgetInspector;
    WidgetInspectorPanel *m_inspectorPanel;

    QLineEdit *m_searchInput;
    QListWidget *m_themeList;
    QPushButton *m_addBtn;
    QPushButton *m_saveBtn;
    QPushButton *m_duplicateBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_exportBtn;
    QPushButton *m_importBtn;
    QPushButton *m_resetBtn;

    QLabel *m_themeCountLabel;

    bool m_isUpdatingPickers = false;
};
