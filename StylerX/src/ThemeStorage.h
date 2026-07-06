#pragma once

#include "ThemeData.h"
#include <QString>
#include <QVector>
#include <QJsonObject>

class ThemeStorage {
public:
    static ThemeStorage &instance();

    bool saveTheme(const ThemeData &theme, const QString &filePath);
    ThemeData loadTheme(const QString &filePath);
    bool deleteTheme(const QString &filePath);
    QStringList listThemes(const QString &directory);
    bool themeExists(const QString &filePath) const;

    static QJsonObject themeToJson(const ThemeData &theme);
    static ThemeData themeFromJson(const QJsonObject &json, const QString &name = "");
    static QString colorToHex(const QColor &color);
    static QColor colorFromHex(const QString &hex);
    static bool isValidHex(const QString &hex);

private:
    ThemeStorage() = default;
    ~ThemeStorage() = default;
    ThemeStorage(const ThemeStorage &) = delete;
    ThemeStorage &operator=(const ThemeStorage &) = delete;
};
