#ifndef SCR_SETTINGS_H
#define SCR_SETTINGS_H

#include "lvgl.h"

extern lv_obj_t *ui_scrSettings;

void scr_settings_prepare(void);
void scr_settings_init(void);
void scr_settings_uninit(void);
void scr_settings_step(void);
void scr_settings_cycle_brightness(void);

#endif
