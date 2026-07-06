#include "SettingsManager.h"

SettingsManager &SettingsManager::instance() {
    static SettingsManager inst;
    return inst;
}

SettingsManager::SettingsManager()
    : m_settings(settingsFilePath(), QSettings::IniFormat) {
    load();
}

QString SettingsManager::configDir() const {
#ifdef _WIN32
    return qEnvironmentVariable("APPDATA") + "/StylerX";
#else
    return QDir::homePath() + "/.config/stylerx";
#endif
}

QString SettingsManager::settingsFilePath() const {
    QDir dir(configDir());
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.absoluteFilePath("settings.ini");
}

void SettingsManager::load() {
    m_settings.beginGroup("General");
    m_lastThemeId = m_settings.value("lastThemeId", "").toString();
    m_dockVisible = m_settings.value("dockVisible", true).toBool();
    m_inspectorEnabled = m_settings.value("inspectorEnabled", false).toBool();
    m_firstLaunch = m_settings.value("firstLaunch", true).toBool();
    m_settings.endGroup();

    m_settings.beginGroup("Dock");
    m_dockGeometry = m_settings.value("geometry").toByteArray();
    m_settings.endGroup();

    m_settings.beginGroup("Recent");
    int count = m_settings.value("count", 0).toInt();
    for (int i = 0; i < count; ++i) {
        m_recentThemes.append(m_settings.value(QString("theme_%1").arg(i)).toString());
    }
    m_settings.endGroup();
}

void SettingsManager::save() {
    m_settings.beginGroup("General");
    m_settings.setValue("lastThemeId", m_lastThemeId);
    m_settings.setValue("dockVisible", m_dockVisible);
    m_settings.setValue("inspectorEnabled", m_inspectorEnabled);
    m_settings.setValue("firstLaunch", m_firstLaunch);
    m_settings.endGroup();

    m_settings.beginGroup("Dock");
    m_settings.setValue("geometry", m_dockGeometry);
    m_settings.endGroup();

    m_settings.beginGroup("Recent");
    m_settings.setValue("count", m_recentThemes.size());
    for (int i = 0; i < m_recentThemes.size(); ++i) {
        m_settings.setValue(QString("theme_%1").arg(i), m_recentThemes[i]);
    }
    m_settings.endGroup();

    m_settings.sync();
}

QString SettingsManager::lastThemeId() const { return m_lastThemeId; }
void SettingsManager::setLastThemeId(const QString &id) {
    m_lastThemeId = id;
    save();
}

QByteArray SettingsManager::dockGeometry() const { return m_dockGeometry; }
void SettingsManager::setDockGeometry(const QByteArray &geo) {
    m_dockGeometry = geo;
    save();
}

bool SettingsManager::dockVisible() const { return m_dockVisible; }
void SettingsManager::setDockVisible(bool visible) {
    m_dockVisible = visible;
    save();
}

bool SettingsManager::inspectorEnabled() const { return m_inspectorEnabled; }
void SettingsManager::setInspectorEnabled(bool enabled) {
    m_inspectorEnabled = enabled;
    save();
}

bool SettingsManager::isFirstLaunch() const { return m_firstLaunch; }
void SettingsManager::setFirstLaunchComplete() {
    m_firstLaunch = false;
    save();
}

QStringList SettingsManager::recentThemes() const { return m_recentThemes; }

void SettingsManager::addRecentTheme(const QString &id) {
    m_recentThemes.removeAll(id);
    m_recentThemes.prepend(id);
    while (m_recentThemes.size() > 10) {
        m_recentThemes.removeLast();
    }
    save();
}
