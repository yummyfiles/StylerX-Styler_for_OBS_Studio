#pragma once

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

class SettingsManager {
public:
    static SettingsManager &instance();

    QString lastThemeId() const;
    void setLastThemeId(const QString &id);

    QByteArray dockGeometry() const;
    void setDockGeometry(const QByteArray &geo);

    bool dockVisible() const;
    void setDockVisible(bool visible);

    bool inspectorEnabled() const;
    void setInspectorEnabled(bool enabled);

    QStringList recentThemes() const;
    void addRecentTheme(const QString &id);

    bool isFirstLaunch() const;
    void setFirstLaunchComplete();

    void save();
    void load();

private:
    SettingsManager();
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager &) = delete;
    SettingsManager &operator=(const SettingsManager &) = delete;

    QString settingsFilePath() const;
    QString configDir() const;

    QSettings m_settings;
    QString m_lastThemeId;
    QByteArray m_dockGeometry;
    bool m_dockVisible = true;
    bool m_inspectorEnabled = false;
    bool m_firstLaunch = true;
    QStringList m_recentThemes;
};
