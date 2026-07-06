#pragma once

#include "ThemeData.h"
#include <QObject>
#include <QMap>
#include <QVector>
#include <QString>
#include <QSet>

class ThemeLibraryManager : public QObject {
    Q_OBJECT
public:
    static ThemeLibraryManager &instance();

    QString themesDirectory() const;
    bool ensureThemesDirectoryExists();

    void loadUserThemes();
    QVector<ThemeData> allThemes() const;
    QVector<ThemeData> favoriteThemes() const;
    QVector<ThemeData> searchThemes(const QString &query) const;

    bool themeExists(const QString &id) const;
    const ThemeData *findTheme(const QString &id) const;
    QString themeFilePath(const QString &id) const;

    bool saveTheme(const ThemeData &theme);
    bool deleteTheme(const QString &id);
    bool renameTheme(const QString &id, const QString &newName);
    bool duplicateTheme(const QString &id);
    bool setFavorite(const QString &id, bool favorite);
    bool importTheme(const QString &filePath);
    bool exportTheme(const QString &id, const QString &destinationPath);

    ThemeData createNewTheme(const QString &name) const;

    QStringList themeFilePaths() const;
    int themeCount() const;

    ThemeData validateAndLoadTheme(const QString &filePath, QString &errorOut);

signals:
    void themeListChanged();
    void themeSaved(const QString &id);
    void themeDeleted(const QString &id);
    void themeImported(const QString &id);

private:
    ThemeLibraryManager();
    ~ThemeLibraryManager() = default;
    ThemeLibraryManager(const ThemeLibraryManager &) = delete;
    ThemeLibraryManager &operator=(const ThemeLibraryManager &) = delete;

    QMap<QString, ThemeData> m_themes;
};
