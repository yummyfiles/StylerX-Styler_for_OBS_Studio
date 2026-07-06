#pragma once

#include "ThemeData.h"
#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager &instance();

    void initialize();

    void addTheme(const ThemeData &theme);
    void removeTheme(const QString &id);
    void renameTheme(const QString &id, const QString &newName);
    void duplicateTheme(const QString &id);
    void setFavorite(const QString &id, bool favorite);

    const ThemeData *currentTheme() const;
    QString currentThemeId() const { return m_currentThemeId; }
    bool setCurrentTheme(const QString &id);

    ThemeData *mutableTheme(const QString &id);
    const ThemeData *findTheme(const QString &id) const;
    QVector<const ThemeData*> allThemes() const;
    QVector<const ThemeData*> favoriteThemes() const;
    QVector<const ThemeData*> searchThemes(const QString &query) const;

    void updateColor(const QString &themeId, const QString &colorKey, const QColor &color);
    void applyCurrentTheme();
    void applyColorsFromTheme(const ThemeData &theme);
    void resetToDefaults();
    void createFromObsAppearance();
    void saveThemeToDisk(const QString &id);

    ThemeData createThemeFromCurrent() const;
    bool hasUserThemes() const;

signals:
    void themeAdded(const QString &id);
    void themeRemoved(const QString &id);
    void themeRenamed(const QString &id, const QString &newName);
    void themeDuplicated(const QString &originalId, const QString &newId);
    void currentThemeChanged(const QString &id);
    void colorUpdated(const QString &themeId, const QString &colorKey, const QColor &color);
    void themeListChanged();
    void themeSaved(const QString &id);

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager &) = delete;
    ThemeManager &operator=(const ThemeManager &) = delete;

    void loadThemes();

    QString m_currentThemeId;
    QMap<QString, ThemeData> m_themes;
    ThemeData m_obsAppearance;
};
