#pragma once

#include "ThemeData.h"
#include <QString>
#include <QHash>
#include <QColor>
#include <QMutex>

class StyleParser {
public:
    static StyleParser &instance();

    QString parseTheme(const ThemeData &theme);
    QString updateColor(const QString &existingStyle, const QString &colorKey, const QColor &newColor);

    void clearCache();
    QString cachedStyle(const QString &themeId) const;
    void cacheStyle(const QString &themeId, const QString &style);

    static QString rgb(const QColor &c);
    static QString rgba(const QColor &c);
    static QString adjustLightness(const QColor &c, qreal factor);

private:
    StyleParser() = default;
    ~StyleParser() = default;
    StyleParser(const StyleParser &) = delete;
    StyleParser &operator=(const StyleParser &) = delete;

    QString buildQSS(const ThemeData &theme) const;
    QString generateGenericWidgets(const ThemeData &t) const;
    QString generateSpecificWidgets(const ThemeData &t) const;

    mutable QMutex m_cacheMutex;
    QHash<QString, QString> m_styleCache;
};
