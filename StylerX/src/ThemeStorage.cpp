#include "ThemeStorage.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDateTime>

ThemeStorage &ThemeStorage::instance() {
    static ThemeStorage inst;
    return inst;
}

QJsonObject ThemeStorage::themeToJson(const ThemeData &theme) {
    QJsonObject obj;
    obj["format_version"] = theme.formatVersion;
    obj["name"] = theme.name;
    obj["author"] = theme.author;
    obj["created"] = theme.created;
    obj["modified"] = theme.modified;
    obj["favorite"] = theme.isFavorite;

    QJsonObject colors;
    for (auto it = theme.colors.constBegin(); it != theme.colors.constEnd(); ++it) {
        colors[it.key()] = colorToHex(it.value());
    }
    obj["colors"] = colors;
    return obj;
}

ThemeData ThemeStorage::themeFromJson(const QJsonObject &json, const QString &name) {
    ThemeData theme;

    if (json.contains("id") && json["id"].isString()) {
        theme.id = json["id"].toString();
    }
    theme.name = name.isEmpty() ? json["name"].toString("Unnamed Theme") : name;
    theme.formatVersion = json["format_version"].toInt(ThemeData::FORMAT_VERSION);
    theme.author = json["author"].toString("");
    theme.created = json["created"].toString(
        QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    theme.modified = json["modified"].toString(
        QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    theme.isFavorite = json["favorite"].toBool(false);

    if (json.contains("colors") && json["colors"].isObject()) {
        QJsonObject colors = json["colors"].toObject();
        for (auto it = colors.constBegin(); it != colors.constEnd(); ++it) {
            QColor c = colorFromHex(it.value().toString());
            if (c.isValid()) {
                theme.colors[it.key()] = c;
            }
        }
    }

    if (theme.colors.isEmpty()) {
        theme = ThemeProperties::createDefaultTheme();
    }

    return theme;
}

QString ThemeStorage::colorToHex(const QColor &color) {
    if (color.alpha() < 255) {
        return color.name(QColor::HexArgb);
    }
    return color.name(QColor::HexRgb);
}

QColor ThemeStorage::colorFromHex(const QString &hex) {
    QString h = hex.trimmed();
    if (h.isEmpty()) return {};

    if (!h.startsWith('#')) {
        h = "#" + h;
    }

    QColor c(h);
    if (c.isValid()) return c;

    if (h.length() == 9) {
        bool ok;
        int alpha = h.mid(1, 2).toInt(&ok, 16);
        QString rgb = "#" + h.mid(3);
        c = QColor(rgb);
        if (c.isValid()) {
            c.setAlpha(alpha);
        }
    }

    return c;
}

bool ThemeStorage::isValidHex(const QString &hex) {
    QString h = hex.trimmed();
    if (!h.startsWith('#')) h = "#" + h;
    QRegularExpression re("^#[0-9a-fA-F]{3}([0-9a-fA-F]{3})?$|"
                          "^#[0-9a-fA-F]{6}([0-9a-fA-F]{2})?$|"
                          "^#[0-9a-fA-F]{4}([0-9a-fA-F]{4})?$");
    return re.match(h).hasMatch();
}

bool ThemeStorage::saveTheme(const ThemeData &theme, const QString &filePath) {
    QJsonObject json = themeToJson(theme);
    QJsonDocument doc(json);

    QFileInfo fi(filePath);
    QDir dir = fi.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

ThemeData ThemeStorage::loadTheme(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return ThemeProperties::createDefaultTheme();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return ThemeProperties::createDefaultTheme();
    }

    QFileInfo fi(filePath);
    QString name = fi.completeBaseName();
    return themeFromJson(doc.object(), name);
}

bool ThemeStorage::deleteTheme(const QString &filePath) {
    return QFile::remove(filePath);
}

QStringList ThemeStorage::listThemes(const QString &directory) {
    QDir dir(directory);
    if (!dir.exists()) return {};

    QStringList filters;
    filters << "*.json";
    QStringList entries = dir.entryList(filters, QDir::Files, QDir::Name);

    QStringList result;
    for (const auto &entry : entries) {
        result << dir.absoluteFilePath(entry);
    }
    return result;
}

bool ThemeStorage::themeExists(const QString &filePath) const {
    return QFileInfo::exists(filePath);
}
