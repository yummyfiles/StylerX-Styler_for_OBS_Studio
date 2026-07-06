# StylerX OBS Frontend API CMake config

get_filename_component(SDK_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

set(OBS_DIR "C:/Program Files/obs-studio")
set(OBS_FRONTEND_INCLUDE_DIRS "${SDK_DIR}/../frontend/api")

add_library(OBSFrontendAPI::OBSFrontendAPI SHARED IMPORTED)
set_target_properties(OBSFrontendAPI::OBSFrontendAPI PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${OBS_FRONTEND_INCLUDE_DIRS}"
    IMPORTED_IMPLIB "${OBS_DIR}/bin/64bit/obs-frontend-api.dll"
    IMPORTED_LOCATION "${OBS_DIR}/bin/64bit/obs-frontend-api.dll"
)
