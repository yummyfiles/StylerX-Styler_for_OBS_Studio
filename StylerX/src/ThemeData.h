#pragma once

#include <QColor>
#include <QMap>
#include <QString>
#include <QVector>
#include <QUuid>
#include <QDateTime>

struct ThemePropertyInfo {
    QString key;
    QString displayName;
    QString description;
    QColor defaultValue;
};

struct ThemeData {
    static const int FORMAT_VERSION = 1;

    QString id;
    QString name;
    QString author;
    QString created;
    QString modified;
    QMap<QString, QColor> colors;
    bool isFavorite = false;
    bool isReadOnly = false;
    int formatVersion = FORMAT_VERSION;

    ThemeData() : id(QUuid::createUuid().toString(QUuid::Id128)) {
        QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        created = now;
        modified = now;
    }

    explicit ThemeData(const QString &name) : ThemeData() { this->name = name; }

    QColor getColor(const QString &key, const QColor &fallback = Qt::black) const {
        return colors.value(key, fallback);
    }

    void setColor(const QString &key, const QColor &color) {
        colors[key] = color;
        markModified();
    }

    void markModified() {
        modified = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    }

    bool isValid() const {
        return !name.isEmpty() && !colors.isEmpty();
    }

    bool operator==(const ThemeData &other) const { return id == other.id; }
    bool operator!=(const ThemeData &other) const { return id != other.id; }
};

namespace ThemeProperties {
inline static QVector<ThemePropertyInfo> all() {
    return {
        {"background",        "Main Background",  "Main window background color",          QColor("#1e1e1e")},
        {"panels",            "Panels",           "Panel background color",                QColor("#252526")},
        {"dock_background",   "Dock Background",  "Dock widget background color",          QColor("#2d2d30")},
        {"buttons",           "Buttons",          "Default button background",             QColor("#3c3c3c")},
        {"button_hover",      "Button Hover",     "Button hover state color",              QColor("#505050")},
        {"button_pressed",    "Button Pressed",   "Button pressed state color",            QColor("#2d2d2d")},
        {"borders",           "Borders",          "Widget border colors",                  QColor("#3c3c3c")},
        {"accent",            "Accent Color",     "Primary accent/highlight color",        QColor("#0078d4")},
        {"selected",          "Selected Items",   "Selected/active item background",        QColor("#264f78")},
        {"text",              "Text",             "Primary text color",                    QColor("#cccccc")},
        {"disabled_text",     "Disabled Text",    "Disabled widget text color",            QColor("#6c6c6c")},
        {"menu_background",   "Menu Background",  "Menu and menubar background",           QColor("#2d2d30")},
        {"menu_text",         "Menu Text",        "Menu item text color",                  QColor("#cccccc")},
        {"tooltips",          "Tooltips",         "Tooltip background color",              QColor("#3c3c3c")},
        {"scrollbars",        "Scrollbars",       "Scrollbar background and handle",       QColor("#424242")},
        {"sliders",           "Sliders",          "Slider groove and handle colors",       QColor("#424242")},
        {"checkboxes",        "Checkboxes",       "Checkbox and radio button colors",      QColor("#3c3c3c")},
        {"input_fields",      "Input Fields",     "Line edit, spinbox, combobox fields",   QColor("#3c3c3c")},
        {"tabs",              "Tabs",             "Tab bar background and active tab",     QColor("#2d2d30")},
        {"status_bar",        "Status Bar",       "Status bar background color",           QColor("#007acc")},
    };
}

inline static ThemePropertyInfo findProperty(const QString &key) {
    for (const auto &prop : ThemeProperties::all()) {
        if (prop.key == key) return prop;
    }
    return {};
}

inline static ThemeData createDefaultTheme() {
    ThemeData theme("OBS Default");
    for (const auto &prop : ThemeProperties::all()) {
        theme.colors[prop.key] = prop.defaultValue;
    }
    theme.isReadOnly = true;
    theme.formatVersion = ThemeData::FORMAT_VERSION;
    return theme;
}

inline static ThemeData detectCurrentObsTheme() {
    ThemeData detected("Current OBS Appearance");
    for (const auto &prop : ThemeProperties::all()) {
        detected.colors[prop.key] = prop.defaultValue;
    }
    detected.isReadOnly = false;
    detected.formatVersion = ThemeData::FORMAT_VERSION;
    return detected;
}
} // namespace ThemeProperties
