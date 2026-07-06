# StylerX OBS SDK CMake config

get_filename_component(SDK_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

set(OBS_INCLUDE_DIRS "${SDK_DIR}/../libobs")
set(OBS_LIB_DIR "${SDK_DIR}/../lib")

add_library(libobs::libobs SHARED IMPORTED)
set_target_properties(libobs::libobs PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${OBS_INCLUDE_DIRS}"
    IMPORTED_IMPLIB "${OBS_LIB_DIR}/obs.lib"
    IMPORTED_LOCATION "C:/Program Files/obs-studio/bin/64bit/obs.dll"
)
