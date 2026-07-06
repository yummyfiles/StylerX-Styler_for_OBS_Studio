#pragma once

#include "ThemeData.h"
#include <QObject>
#include <QString>
#include <QPointer>
#include <QTimer>
#include <QWidget>
#include <QSet>

class ThemeApplier : public QObject {
    Q_OBJECT
public:
    static ThemeApplier &instance();

    void applyTheme(const ThemeData &theme);
    void applyColorUpdate(const ThemeData &theme, const QString &colorKey);

    void setStyleSheet(const QString &styleSheet);
    void applyRawStyleSheet(const QString &qss);
    void clearStylerXStyles();

    void refreshAllWidgets();
    void refreshWidget(QWidget *widget);

    void setDebounceEnabled(bool enabled);
    void setDebounceInterval(int ms);

    static QString wrapStylerXStyle(const QString &qss);

private:
    ThemeApplier();
    ~ThemeApplier() override;
    ThemeApplier(const ThemeApplier &) = delete;
    ThemeApplier &operator=(const ThemeApplier &) = delete;

    QString m_stylerXStyleSheet;
    ThemeData m_currentTheme;
    bool m_debounceEnabled = true;
    int m_debounceInterval = 16;

    QTimer *m_debounceTimer;
    QString m_pendingStyle;
    bool m_hasPending = false;
};
