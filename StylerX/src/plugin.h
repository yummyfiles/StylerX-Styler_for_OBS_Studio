#pragma once

#include <obs-module.h>
#include <obs-frontend-api.h>

extern "C" {

OBS_DECLARE_MODULE();
OBS_MODULE_USE_DEFAULT_LOCALE("styler-x", "en-US");

bool obs_module_load(void);
void obs_module_unload(void);
void obs_module_post_load(void);
const char *obs_module_description(void);
const char *obs_module_name(void);
}
