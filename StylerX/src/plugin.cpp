#include "plugin.h"
#include "ThemeManager.h"
#include "ThemeLibraryManager.h"
#include "ThemeApplier.h"
#include "ThemeEditorDock.h"
#include <QMainWindow>
#include <QAction>
#include <QApplication>

static ThemeEditorDock *s_dock = nullptr;
static QAction *s_toggleAction = nullptr;

static void toggleDockVisibility() {
    if (s_dock) {
        s_dock->setVisible(!s_dock->isVisible());
    }
}

static void saveDockGeometry() {
    if (s_dock) {
        s_dock->saveSettings();
    }
}

obs_module_t *obs_module_handle = nullptr;

bool obs_module_load(void) {
    blog(LOG_INFO, "[StylerX] Loading StylerX plugin v%s", PLUGIN_VERSION);
    return true;
}

void obs_module_post_load(void) {
    blog(LOG_INFO, "[StylerX] Initializing StylerX UI...");

    QMainWindow *mainWindow = reinterpret_cast<QMainWindow*>(
        obs_frontend_get_main_window());
    if (!mainWindow) {
        blog(LOG_WARNING, "[StylerX] Could not get OBS main window");
        return;
    }

    ThemeLibraryManager::instance().loadUserThemes();
    ThemeManager::instance().initialize();

    s_dock = new ThemeEditorDock(mainWindow);
    s_dock->setVisible(false);

    obs_frontend_add_custom_qdock("stylerx_dock", s_dock);

    s_toggleAction = reinterpret_cast<QAction*>(
        obs_frontend_add_tools_menu_qaction("StylerX Studio"));
    if (s_toggleAction) {
        QObject::connect(s_toggleAction, &QAction::triggered, toggleDockVisibility);
    }

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, saveDockGeometry);

    blog(LOG_INFO, "[StylerX] StylerX plugin loaded successfully");
}

void obs_module_unload(void) {
    blog(LOG_INFO, "[StylerX] Unloading StylerX plugin...");

    QObject::disconnect(qApp, &QCoreApplication::aboutToQuit, nullptr, nullptr);

    ThemeApplier::instance().shutdown();
    ThemeApplier::instance().clearStylerXStyles();

    if (s_dock) {
        s_dock->cleanup();
        s_dock->saveSettings();
        s_dock->deleteLater();
        s_dock = nullptr;
    }

    if (s_toggleAction) {
        delete s_toggleAction;
        s_toggleAction = nullptr;
    }

    blog(LOG_INFO, "[StylerX] StylerX plugin unloaded");
}

const char *obs_module_description(void) {
    return "StylerX - Live theme editor for OBS Studio. "
           "Create, customize, and preview themes in real time.";
}

const char *obs_module_name(void) {
    return "StylerX";
}
