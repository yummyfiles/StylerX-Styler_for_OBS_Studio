#include "ThemeApplier.h"
#include "StyleParser.h"
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStyle>
#include <QDebug>

ThemeApplier &ThemeApplier::instance() {
    static ThemeApplier inst;
    return inst;
}

ThemeApplier::ThemeApplier()
    : m_debounceTimer(new QTimer(this)) {
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(m_debounceInterval);
    QObject::connect(m_debounceTimer, &QTimer::timeout, [this]() {
        if (m_hasPending) {
            m_hasPending = false;
            setStyleSheet(m_pendingStyle);
        }
    });
}

ThemeApplier::~ThemeApplier() {}

QString ThemeApplier::wrapStylerXStyle(const QString &qss) {
    return "/* StylerX - Layered Theme Layer */\n"
           "/* This stylesheet is applied as a layer on top of OBS base styles */\n"
           "/* It only overrides properties explicitly set by the user */\n\n"
           + qss;
}

void ThemeApplier::setDebounceEnabled(bool enabled) {
    m_debounceEnabled = enabled;
}

void ThemeApplier::setDebounceInterval(int ms) {
    m_debounceInterval = qBound(8, ms, 1000);
    m_debounceTimer->setInterval(m_debounceInterval);
}

void ThemeApplier::applyTheme(const ThemeData &theme) {
    m_currentTheme = theme;
    QString style = StyleParser::instance().parseTheme(theme);
    QString wrapped = wrapStylerXStyle(style);
    setStyleSheet(wrapped);
}

void ThemeApplier::applyColorUpdate(const ThemeData &theme, const QString &colorKey) {
    m_currentTheme = theme;

    QColor newColor = theme.colors.value(colorKey);
    QString updatedStyle = StyleParser::instance().updateColor(
        m_stylerXStyleSheet, colorKey, newColor);

    if (m_debounceEnabled) {
        m_pendingStyle = wrapStylerXStyle(updatedStyle);
        m_hasPending = true;
        m_debounceTimer->start();
    } else {
        setStyleSheet(wrapStylerXStyle(updatedStyle));
    }
}

void ThemeApplier::setStyleSheet(const QString &styleSheet) {
    m_stylerXStyleSheet = styleSheet;

    for (QWidget *widget : QApplication::topLevelWidgets()) {
        widget->setStyleSheet(styleSheet);
    }

    refreshAllWidgets();
}

void ThemeApplier::applyRawStyleSheet(const QString &qss) {
    setStyleSheet(wrapStylerXStyle(qss));
}

void ThemeApplier::clearStylerXStyles() {
    m_stylerXStyleSheet.clear();

    for (QWidget *widget : QApplication::topLevelWidgets()) {
        widget->setStyleSheet("");
    }

    refreshAllWidgets();
}

void ThemeApplier::refreshAllWidgets() {
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        refreshWidget(widget);
    }
}

void ThemeApplier::refreshWidget(QWidget *widget) {
    if (!widget) return;

    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();

    for (QObject *child : widget->children()) {
        QWidget *childWidget = qobject_cast<QWidget*>(child);
        if (childWidget) {
            refreshWidget(childWidget);
        }
    }
}
