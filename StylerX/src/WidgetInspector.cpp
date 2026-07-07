#include "WidgetInspector.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QSet>

/* ---- WidgetInspector ---- */

WidgetInspector::WidgetInspector(QObject *parent)
    : QObject(parent) {
}

void WidgetInspector::setEnabled(bool enabled) {
    if (m_enabled == enabled) return;
    m_enabled = enabled;

    if (enabled) {
        qApp->installEventFilter(this);
    } else {
        qApp->removeEventFilter(this);
        clearHighlights();
    }
}

void WidgetInspector::clearHighlights() {
    if (m_highlightOverlay) {
        m_highlightOverlay->hide();
        m_highlightOverlay->deleteLater();
        m_highlightOverlay = nullptr;
    }
    m_hoveredWidget = nullptr;
}

bool WidgetInspector::eventFilter(QObject *obj, QEvent *event) {
    if (!m_enabled) return false;

    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget) return false;

    {
        QWidget *p = widget;
        while (p) {
            QString name = p->objectName();
            if (name == "StylerXDock" || name == "stylerx_dock") {
                clearHighlights();
                m_hoveredWidget = nullptr;
                return false;
            }
            p = p->parentWidget();
        }
    }

    switch (event->type()) {
    case QEvent::Enter:
        if (QWidget *w = qobject_cast<QWidget*>(obj)) {
            if (m_hoveredWidget && m_hoveredWidget != w) {
                highlightWidget(m_hoveredWidget, false);
            }
            m_hoveredWidget = w;
            highlightWidget(w, true);
        }
        break;

    case QEvent::Leave:
        if (m_hoveredWidget) {
            highlightWidget(m_hoveredWidget, false);
            m_hoveredWidget = nullptr;
        }
        break;

    case QEvent::MouseButtonPress:
        if (QMouseEvent *me = dynamic_cast<QMouseEvent*>(event)) {
            if (me->button() == Qt::LeftButton) {
                if (QWidget *w = qobject_cast<QWidget*>(obj)) {
                    inspectWidget(w);
                    return true;
                }
            }
        }
        break;

    default:
        break;
    }

    return false;
}

void WidgetInspector::highlightWidget(QWidget *widget, bool highlight) {
    if (!widget) return;

    if (!highlight) {
        widget->update();
        return;
    }

    widget->update();

    if (m_highlightOverlay) {
        m_highlightOverlay->hide();
        m_highlightOverlay->deleteLater();
        m_highlightOverlay = nullptr;
    }

    QWidget *overlay = new QWidget(widget->window());
    overlay->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowTransparentForInput);
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    overlay->setAttribute(Qt::WA_ShowWithoutActivating, true);
    overlay->setAttribute(Qt::WA_TranslucentBackground, true);

    int r, g, b, br, bg, bb;
    m_highlightColor.getRgb(&r, &g, &b);
    m_borderColor.getRgb(&br, &bg, &bb);

    overlay->setStyleSheet(QString(
        "background-color: rgba(%1, %2, %3, 32);"
        "border: 2px solid rgba(%4, %5, %6, 200);"
    ).arg(r).arg(g).arg(b).arg(br).arg(bg).arg(bb));

    QPoint globalPos = widget->mapToGlobal(QPoint(0, 0));
    QSize size = widget->size();
    overlay->setGeometry(globalPos.x(), globalPos.y(), size.width(), size.height());
    overlay->raise();
    overlay->show();

    m_highlightOverlay = overlay;
}

void WidgetInspector::inspectWidget(QWidget *widget) {
    if (!widget) return;

    QString className = widget->metaObject()->className();
    QString objectName = widget->objectName();
    QString styleSheet = widget->styleSheet();
    QString colorInfo = extractColors(styleSheet);

    QWidget *parent = widget->parentWidget();
    while (parent) {
        QString ps = parent->styleSheet();
        if (!ps.isEmpty()) {
            if (!styleSheet.isEmpty()) styleSheet += "\n\n/* From parent " + QString(parent->metaObject()->className()) + " */\n";
            styleSheet += ps;
            QString pc = extractColors(ps);
            if (!pc.isEmpty()) {
                if (!colorInfo.isEmpty()) colorInfo += "\n";
                colorInfo += pc;
            }
        }
        parent = parent->parentWidget();
    }

    emit widgetInspected(className, objectName, styleSheet, colorInfo);
}

QString WidgetInspector::extractColors(const QString &styleSheet) {
    if (styleSheet.isEmpty()) return {};

    QStringList colors;
    QRegularExpression hexRe("#[0-9a-fA-F]{3,8}\\b");
    QRegularExpressionMatchIterator it = hexRe.globalMatch(styleSheet);
    QSet<QString> found;
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString hex = m.captured();
        if (!found.contains(hex)) {
            found.insert(hex);
            colors << hex;
        }
    }

    QRegularExpression rgbRe("rgb[a]?\\([^)]+\\)");
    QRegularExpressionMatchIterator rit = rgbRe.globalMatch(styleSheet);
    while (rit.hasNext()) {
        QRegularExpressionMatch m = rit.next();
        QString rgb = m.captured();
        if (!found.contains(rgb)) {
            found.insert(rgb);
            colors << rgb;
        }
    }

    return colors.join(", ");
}

/* ---- WidgetInspectorPanel ---- */

WidgetInspectorPanel::WidgetInspectorPanel(QWidget *parent)
    : QWidget(parent) {
    setupUI();
}

void WidgetInspectorPanel::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    m_enableCheck = new QCheckBox("Enable Inspector Mode", this);
    m_enableCheck->setStyleSheet("QCheckBox { color: #ccc; font-weight: bold; }");
    connect(m_enableCheck, &QCheckBox::toggled, this, &WidgetInspectorPanel::onToggleInspector);
    layout->addWidget(m_enableCheck);

    QGroupBox *infoGroup = new QGroupBox("Widget Info", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    infoLayout->setSpacing(4);

    m_classLabel = new QLabel("Class: -", this);
    m_objectNameLabel = new QLabel("Object Name: -", this);
    m_classLabel->setStyleSheet("color: #ccc; font-size: 11px;");
    m_objectNameLabel->setStyleSheet("color: #ccc; font-size: 11px;");
    infoLayout->addWidget(m_classLabel);
    infoLayout->addWidget(m_objectNameLabel);

    layout->addWidget(infoGroup);

    QGroupBox *styleGroup = new QGroupBox("Stylesheet Rules", this);
    QVBoxLayout *styleLayout = new QVBoxLayout(styleGroup);
    m_styleSheetView = new QTextEdit(this);
    m_styleSheetView->setReadOnly(true);
    m_styleSheetView->setMaximumHeight(120);
    m_styleSheetView->setStyleSheet(
        "QTextEdit { background-color: #1e1e1e; color: #d4d4d4; "
        "border: 1px solid #3c3c3c; font-family: Consolas; font-size: 11px; }");
    styleLayout->addWidget(m_styleSheetView);
    layout->addWidget(styleGroup);

    QGroupBox *colorGroup = new QGroupBox("Colors in Use", this);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorGroup);
    m_colorView = new QTextEdit(this);
    m_colorView->setReadOnly(true);
    m_colorView->setMaximumHeight(80);
    m_colorView->setStyleSheet(
        "QTextEdit { background-color: #1e1e1e; color: #f9e2af; "
        "border: 1px solid #3c3c3c; font-family: Consolas; font-size: 11px; }");
    colorLayout->addWidget(m_colorView);
    layout->addWidget(colorGroup);

    m_editButton = new QPushButton("Edit These Colors", this);
    m_editButton->setEnabled(false);
    m_editButton->setStyleSheet(
        "QPushButton { background-color: #0078d4; color: white; "
        "border: none; padding: 6px; border-radius: 3px; }"
        "QPushButton:hover { background-color: #1a8ad4; }"
        "QPushButton:disabled { background-color: #3c3c3c; color: #6c6c6c; }");
    connect(m_editButton, &QPushButton::clicked, [this]() {
        emit editColorRequested("");
    });
    layout->addWidget(m_editButton);

    layout->addStretch();
}

void WidgetInspectorPanel::setInspector(WidgetInspector *inspector) {
    m_inspector = inspector;
    if (m_inspector) {
        connect(m_inspector, &WidgetInspector::widgetInspected,
                this, &WidgetInspectorPanel::onWidgetInspected);
    }
}

void WidgetInspectorPanel::onToggleInspector(bool enabled) {
    if (m_inspector) {
        m_inspector->setEnabled(enabled);
    }
    if (!enabled) {
        m_classLabel->setText("Class: -");
        m_objectNameLabel->setText("Object Name: -");
        m_styleSheetView->clear();
        m_colorView->clear();
        m_editButton->setEnabled(false);
    }
}

void WidgetInspectorPanel::onWidgetInspected(const QString &className,
                                              const QString &objectName,
                                              const QString &styleSheet,
                                              const QString &colorInfo) {
    m_classLabel->setText("Class: " + className);
    m_objectNameLabel->setText("Object Name: " + (objectName.isEmpty() ? "(unnamed)" : objectName));
    m_styleSheetView->setPlainText(styleSheet.isEmpty() ? "(no direct stylesheet)" : styleSheet);
    m_colorView->setPlainText(colorInfo.isEmpty() ? "(no colors detected)" : colorInfo);
    m_editButton->setEnabled(!colorInfo.isEmpty());
}
