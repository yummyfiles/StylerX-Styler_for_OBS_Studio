#include "ThemeLibraryManager.h"
#include "ThemeStorage.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QStandardPaths>

ThemeLibraryManager &ThemeLibraryManager::instance() {
    static ThemeLibraryManager inst;
    return inst;
}

ThemeLibraryManager::ThemeLibraryManager() {}

QString ThemeLibraryManager::themesDirectory() const {
#ifdef _WIN32
    return qEnvironmentVariable("APPDATA") + "/StylerX/themes";
#else
    QString home = QDir::homePath();
    return home + "/.config/stylerx/themes";
#endif
}

bool ThemeLibraryManager::ensureThemesDirectoryExists() {
    QDir dir(themesDirectory());
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

void ThemeLibraryManager::loadUserThemes() {
    m_themes.clear();

    QString dir = themesDirectory();
    QDir d(dir);
    if (!d.exists()) return;

    QStringList files = d.entryList({"*.json"}, QDir::Files, QDir::Name);
    for (const QString &file : files) {
        QString filePath = d.absoluteFilePath(file);
        ThemeData theme = ThemeStorage::instance().loadTheme(filePath);
        if (theme.isValid()) {
            m_themes[theme.id] = theme;
        }
    }
}

QVector<ThemeData> ThemeLibraryManager::allThemes() const {
    QVector<ThemeData> result;
    result.reserve(m_themes.size());
    for (auto it = m_themes.constBegin(); it != m_themes.constEnd(); ++it) {
        result.append(it.value());
    }
    return result;
}

QVector<ThemeData> ThemeLibraryManager::favoriteThemes() const {
    QVector<ThemeData> result;
    for (auto it = m_themes.constBegin(); it != m_themes.constEnd(); ++it) {
        if (it.value().isFavorite) {
            result.append(it.value());
        }
    }
    return result;
}

QVector<ThemeData> ThemeLibraryManager::searchThemes(const QString &query) const {
    if (query.isEmpty()) return allThemes();

    QVector<ThemeData> result;
    QString q = query.toLower();
    for (auto it = m_themes.constBegin(); it != m_themes.constEnd(); ++it) {
        if (it.value().name.toLower().contains(q)) {
            result.append(it.value());
            continue;
        }
        for (auto cit = it.value().colors.constBegin(); cit != it.value().colors.constEnd(); ++cit) {
            if (cit.key().toLower().contains(q) ||
                cit.value().name().contains(q, Qt::CaseInsensitive)) {
                result.append(it.value());
                break;
            }
        }
    }
    return result;
}

bool ThemeLibraryManager::themeExists(const QString &id) const {
    return m_themes.contains(id);
}

const ThemeData *ThemeLibraryManager::findTheme(const QString &id) const {
    auto it = m_themes.constFind(id);
    if (it != m_themes.constEnd()) {
        return &it.value();
    }
    return nullptr;
}

QString ThemeLibraryManager::themeFilePath(const QString &id) const {
    return themesDirectory() + "/" + id + ".json";
}

bool ThemeLibraryManager::saveTheme(const ThemeData &theme) {
    ensureThemesDirectoryExists();
    ThemeData copy = theme;
    copy.markModified();

    QString filePath = themeFilePath(theme.id);
    if (ThemeStorage::instance().saveTheme(copy, filePath)) {
        m_themes[theme.id] = copy;
        emit themeSaved(theme.id);
        emit themeListChanged();
        return true;
    }
    return false;
}

bool ThemeLibraryManager::deleteTheme(const QString &id) {
    if (!m_themes.contains(id)) return false;
    if (m_themes[id].isReadOnly) return false;

    QString filePath = themeFilePath(id);
    QFile::remove(filePath);
    m_themes.remove(id);

    emit themeDeleted(id);
    emit themeListChanged();
    return true;
}

bool ThemeLibraryManager::renameTheme(const QString &id, const QString &newName) {
    if (!m_themes.contains(id)) return false;
    if (m_themes[id].isReadOnly) return false;

    ThemeData copy = m_themes[id];
    copy.name = newName;
    copy.markModified();
    return saveTheme(copy);
}

bool ThemeLibraryManager::duplicateTheme(const QString &id) {
    if (!m_themes.contains(id)) return false;

    ThemeData copy = m_themes[id];
    copy.id = QUuid::createUuid().toString(QUuid::Id128);
    copy.name = copy.name + " (Copy)";
    copy.isReadOnly = false;
    copy.isFavorite = false;
    copy.formatVersion = ThemeData::FORMAT_VERSION;
    copy.markModified();

    return saveTheme(copy);
}

bool ThemeLibraryManager::setFavorite(const QString &id, bool favorite) {
    if (!m_themes.contains(id)) return false;

    ThemeData copy = m_themes[id];
    copy.isFavorite = favorite;
    copy.markModified();
    return saveTheme(copy);
}

bool ThemeLibraryManager::importTheme(const QString &filePath) {
    QString error;
    ThemeData theme = validateAndLoadTheme(filePath, error);
    if (!error.isEmpty()) return false;

    theme.id = QUuid::createUuid().toString(QUuid::Id128);
    theme.isReadOnly = false;
    theme.markModified();
    return saveTheme(theme);
}

bool ThemeLibraryManager::exportTheme(const QString &id, const QString &destinationPath) {
    if (!m_themes.contains(id)) return false;
    return ThemeStorage::instance().saveTheme(m_themes[id], destinationPath);
}

ThemeData ThemeLibraryManager::createNewTheme(const QString &name) const {
    ThemeData theme(name);
    theme.formatVersion = ThemeData::FORMAT_VERSION;
    for (const auto &prop : ThemeProperties::all()) {
        theme.colors[prop.key] = prop.defaultValue;
    }
    return theme;
}

QStringList ThemeLibraryManager::themeFilePaths() const {
    QDir dir(themesDirectory());
    if (!dir.exists()) return {};
    return dir.entryList({"*.json"}, QDir::Files, QDir::Name);
}

int ThemeLibraryManager::themeCount() const {
    return m_themes.size();
}

ThemeData ThemeLibraryManager::validateAndLoadTheme(const QString &filePath, QString &errorOut) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorOut = "Could not open file: " + filePath;
        return {};
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        errorOut = "Invalid JSON: " + QString(parseError.errorString());
        return {};
    }

    if (!doc.isObject()) {
        errorOut = "Theme file must contain a JSON object at the top level.";
        return {};
    }

    QJsonObject json = doc.object();

    int formatVersion = json["format_version"].toInt(0);
    if (formatVersion == 0) {
        errorOut = "Missing or invalid 'format_version'. Expected " +
                   QString::number(ThemeData::FORMAT_VERSION) + ".";
        return {};
    }

    if (formatVersion > ThemeData::FORMAT_VERSION) {
        errorOut = "Theme format version " + QString::number(formatVersion) +
                   " is too new. This version of StylerX supports up to v" +
                   QString::number(ThemeData::FORMAT_VERSION) + ".";
        return {};
    }

    if (!json.contains("colors") || !json["colors"].isObject()) {
        errorOut = "Missing 'colors' object in theme file.";
        return {};
    }

    QJsonObject colors = json["colors"].toObject();
    if (colors.isEmpty()) {
        errorOut = "Theme contains no color definitions.";
        return {};
    }

    QStringList invalidColors;
    for (auto it = colors.constBegin(); it != colors.constEnd(); ++it) {
        QString hex = it.value().toString();
        if (!hex.startsWith('#')) hex = "#" + hex;
        QColor c(hex);
        if (!c.isValid()) {
            invalidColors << (it.key() + ": " + it.value().toString());
        }
    }

    if (!invalidColors.isEmpty()) {
        errorOut = "Invalid color values found:\n" + invalidColors.join("\n");
        return {};
    }

    QString name = json["name"].toString(QFileInfo(filePath).completeBaseName());
    ThemeData theme = ThemeStorage::themeFromJson(json, name);
    theme.formatVersion = formatVersion;
    return theme;
}
