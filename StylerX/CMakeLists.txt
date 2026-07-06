cmake_minimum_required(VERSION 3.22)
project(styler-x-plugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(libobs REQUIRED)

set(OBS_SDK_DIR "${CMAKE_SOURCE_DIR}/deps/obs-sdk")
set(OBS_FRONTEND_INCLUDE_DIR "${OBS_SDK_DIR}/frontend/api")

# If OBSFrontendAPI isn't found via find_package, set it up manually
if(NOT OBSFrontendAPI_FOUND)
    add_library(OBSFrontendAPI::OBSFrontendAPI SHARED IMPORTED)
    set_target_properties(OBSFrontendAPI::OBSFrontendAPI PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OBS_FRONTEND_INCLUDE_DIR}"
        IMPORTED_IMPLIB "${OBS_SDK_DIR}/lib/obs-frontend-api.lib"
        IMPORTED_LOCATION "C:/Program Files/obs-studio/bin/64bit/obs-frontend-api.dll"
    )
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

add_library(styler-x MODULE
    src/plugin.cpp
    src/ThemeData.h
    src/ThemeManager.cpp
    src/ThemeManager.h
    src/ThemeStorage.cpp
    src/ThemeStorage.h
    src/StyleParser.cpp
    src/StyleParser.h
    src/ThemeApplier.cpp
    src/ThemeApplier.h
    src/ThemeLibraryManager.cpp
    src/ThemeLibraryManager.h
    src/SettingsManager.cpp
    src/SettingsManager.h
    src/ColorPickerWidget.cpp
    src/ColorPickerWidget.h
    src/QSSEditorWidget.cpp
    src/QSSEditorWidget.h
    src/WidgetInspector.cpp
    src/WidgetInspector.h
    src/ThemeEditorDock.cpp
    src/ThemeEditorDock.h
)

target_include_directories(styler-x PRIVATE src)
target_link_libraries(styler-x PRIVATE libobs::libobs OBSFrontendAPI::OBSFrontendAPI Qt6::Widgets Qt6::Core)

target_compile_definitions(styler-x PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")

if(WIN32)
    set_target_properties(styler-x PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(styler-x PROPERTIES SUFFIX ".bundle")
else()
    set_target_properties(styler-x PROPERTIES SUFFIX ".so")
endif()

set_target_properties(styler-x PROPERTIES PREFIX "")

if(OBS_PLUGIN_DESTINATION)
    install(TARGETS styler-x
        LIBRARY DESTINATION "${OBS_PLUGIN_DESTINATION}"
        RUNTIME DESTINATION "${OBS_PLUGIN_DESTINATION}"
    )
endif()
