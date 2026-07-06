#include "ThemeManager.h"
#include "ThemeStorage.h"
#include "ThemeLibraryManager.h"
#include "StyleParser.h"
#include "ThemeApplier.h"
#include "SettingsManager.h"
#include <QDir>
#include <QFileInfo>

ThemeManager &ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager() {}

void ThemeManager::initialize() {
    m_obsAppearance = ThemeProperties::detectCurrentObsTheme();

    loadThemes();

    if (!hasUserThemes() || SettingsManager::instance().isFirstLaunch()) {
        ThemeData detected = ThemeProperties::detectCurrentObsTheme();
        detected.name = "Current OBS";
        m_themes[detected.id] = detected;
        m_currentThemeId = detected.id;

        if (SettingsManager::instance().isFirstLaunch()) {
            SettingsManager::instance().setFirstLaunchComplete();
        }

        emit themeListChanged();
        emit currentThemeChanged(m_currentThemeId);
        return;
    }

    QString lastId = SettingsManager::instance().lastThemeId();
    if (!lastId.isEmpty() && m_themes.contains(lastId)) {
        m_currentThemeId = lastId;
    } else if (!m_themes.isEmpty()) {
        m_currentThemeId = m_themes.firstKey();
    } else {
        ThemeData detected = ThemeProperties::detectCurrentObsTheme();
        detected.name = "Current OBS";
        m_themes[detected.id] = detected;
        m_currentThemeId = detected.id;
    }

    emit themeListChanged();
    emit currentThemeChanged(m_currentThemeId);
}

void ThemeManager::createFromObsAppearance() {
    ThemeData detected = ThemeProperties::detectCurrentObsTheme();
    detected.name = "My Theme";
    m_themes[detected.id] = detected;
    m_currentThemeId = detected.id;
    emit themeListChanged();
    emit currentThemeChanged(m_currentThemeId);
}

bool ThemeManager::hasUserThemes() const {
    return !m_themes.isEmpty();
}

void ThemeManager::loadThemes() {
    m_themes.clear();

    QVector<ThemeData> userThemes = ThemeLibraryManager::instance().allThemes();
    for (const auto &theme : userThemes) {
        m_themes[theme.id] = theme;
    }
}

void ThemeManager::addTheme(const ThemeData &theme) {
    m_themes[theme.id] = theme;
    ThemeLibraryManager::instance().saveTheme(theme);
    emit themeAdded(theme.id);
    emit themeListChanged();
}

void ThemeManager::removeTheme(const QString &id) {
    if (!m_themes.contains(id)) return;
    if (m_themes[id].isReadOnly) return;

    ThemeLibraryManager::instance().deleteTheme(id);
    m_themes.remove(id);

    if (m_currentThemeId == id) {
        if (!m_themes.isEmpty()) {
            m_currentThemeId = m_themes.firstKey();
        } else {
            ThemeData detected = ThemeProperties::detectCurrentObsTheme();
            detected.name = "Current OBS";
            m_themes[detected.id] = detected;
            m_currentThemeId = detected.id;
        }
        emit currentThemeChanged(m_currentThemeId);
    }

    emit themeRemoved(id);
    emit themeListChanged();
}

void ThemeManager::renameTheme(const QString &id, const QString &newName) {
    if (!m_themes.contains(id)) return;
    if (m_themes[id].isReadOnly) return;

    m_themes[id].name = newName;
    ThemeLibraryManager::instance().renameTheme(id, newName);

    emit themeRenamed(id, newName);
    emit themeListChanged();
}

void ThemeManager::duplicateTheme(const QString &id) {
    if (!m_themes.contains(id)) return;

    ThemeLibraryManager::instance().duplicateTheme(id);
    loadThemes();

    emit themeDuplicated(id, QString());
    emit themeListChanged();
}

void ThemeManager::setFavorite(const QString &id, bool favorite) {
    if (!m_themes.contains(id)) return;
    m_themes[id].isFavorite = favorite;
    ThemeLibraryManager::instance().setFavorite(id, favorite);
    emit themeListChanged();
}

const ThemeData *ThemeManager::currentTheme() const {
    auto it = m_themes.constFind(m_currentThemeId);
    if (it != m_themes.constEnd()) {
        return &it.value();
    }
    return nullptr;
}

bool ThemeManager::setCurrentTheme(const QString &id) {
    if (!m_themes.contains(id)) return false;
    m_currentThemeId = id;
    SettingsManager::instance().setLastThemeId(id);
    SettingsManager::instance().addRecentTheme(id);
    emit currentThemeChanged(id);
    return true;
}

ThemeData *ThemeManager::mutableTheme(const QString &id) {
    auto it = m_themes.find(id);
    if (it != m_themes.end()) {
        return &it.value();
    }
    return nullptr;
}

const ThemeData *ThemeManager::findTheme(const QString &id) const {
    auto it = m_themes.constFind(id);
    if (it != m_themes.constEnd()) {
        return &it.value();
    }
    return nullptr;
}

QVector<const ThemeData*> ThemeManager::allThemes() const {
    QVector<const ThemeData*> result;
    result.reserve(m_themes.size());
    for (const auto &theme : m_themes) {
        result.append(&theme);
    }
    return result;
}

QVector<const ThemeData*> ThemeManager::favoriteThemes() const {
    QVector<const ThemeData*> result;
    for (const auto &theme : m_themes) {
        if (theme.isFavorite) result.append(&theme);
    }
    return result;
}

QVector<const ThemeData*> ThemeManager::searchThemes(const QString &query) const {
    if (query.isEmpty()) return allThemes();

    QVector<const ThemeData*> result;
    QString q = query.toLower();
    for (const auto &theme : m_themes) {
        if (theme.name.toLower().contains(q)) {
            result.append(&theme);
            continue;
        }
        for (auto it = theme.colors.constBegin(); it != theme.colors.constEnd(); ++it) {
            if (it.key().toLower().contains(q) ||
                it.value().name().contains(q, Qt::CaseInsensitive)) {
                result.append(&theme);
                break;
            }
        }
    }
    return result;
}

void ThemeManager::updateColor(const QString &themeId, const QString &colorKey, const QColor &color) {
    auto it = m_themes.find(themeId);
    if (it == m_themes.end()) return;

    it->colors[colorKey] = color;
    it->markModified();

    ThemeApplier::instance().applyColorUpdate(it.value(), colorKey);

    if (!it->isReadOnly) {
        ThemeLibraryManager::instance().saveTheme(it.value());
    }

    emit colorUpdated(themeId, colorKey, color);
}

void ThemeManager::applyCurrentTheme() {
    const ThemeData *theme = currentTheme();
    if (theme) {
        applyColorsFromTheme(*theme);
    }
}

void ThemeManager::applyColorsFromTheme(const ThemeData &theme) {
    ThemeApplier::instance().applyTheme(theme);
}

void ThemeManager::resetToDefaults() {
    m_themes.clear();

    ThemeData detected = ThemeProperties::detectCurrentObsTheme();
    detected.name = "Current OBS";
    m_themes[detected.id] = detected;
    m_currentThemeId = detected.id;

    emit themeListChanged();
    emit currentThemeChanged(m_currentThemeId);
}

ThemeData ThemeManager::createThemeFromCurrent() const {
    const ThemeData *current = currentTheme();
    if (current) {
        ThemeData copy = *current;
        copy.id = QUuid::createUuid().toString(QUuid::Id128);
        copy.name = copy.name + " (Custom)";
        copy.isReadOnly = false;
        copy.formatVersion = ThemeData::FORMAT_VERSION;
        copy.markModified();
        return copy;
    }
    return ThemeProperties::detectCurrentObsTheme();
}

void ThemeManager::saveThemeToDisk(const QString &id) {
    auto it = m_themes.constFind(id);
    if (it == m_themes.constEnd() || it->isReadOnly) return;

    ThemeLibraryManager::instance().saveTheme(it.value());
    emit themeSaved(id);
}
