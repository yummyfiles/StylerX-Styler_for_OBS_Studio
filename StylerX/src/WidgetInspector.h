#pragma once

#include <QWidget>
#include <QObject>
#include <QColor>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QPointer>
#include <QEvent>

class WidgetInspector : public QObject {
    Q_OBJECT
public:
    explicit WidgetInspector(QObject *parent = nullptr);

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    void highlightWidget(QWidget *widget, bool highlight);
    void inspectWidget(QWidget *widget);

signals:
    void widgetInspected(const QString &className, const QString &objectName,
                         const QString &styleSheet, const QString &colorInfo);
    void editColorRequested(const QString &colorKey);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void clearHighlights();
    QString extractColors(const QString &styleSheet);

    bool m_enabled = false;
    QPointer<QWidget> m_hoveredWidget;
    QPointer<QWidget> m_highlightOverlay;
    QColor m_highlightColor{255, 165, 0, 80};
    QColor m_borderColor{255, 165, 0, 200};
};

class WidgetInspectorPanel : public QWidget {
    Q_OBJECT
public:
    explicit WidgetInspectorPanel(QWidget *parent = nullptr);

    void setInspector(WidgetInspector *inspector);

signals:
    void editColorRequested(const QString &colorKey);

private slots:
    void onWidgetInspected(const QString &className, const QString &objectName,
                           const QString &styleSheet, const QString &colorInfo);
    void onToggleInspector(bool enabled);

private:
    void setupUI();

    WidgetInspector *m_inspector = nullptr;
    QCheckBox *m_enableCheck;
    QLabel *m_classLabel;
    QLabel *m_objectNameLabel;
    QTextEdit *m_styleSheetView;
    QTextEdit *m_colorView;
    QPushButton *m_editButton;
};
