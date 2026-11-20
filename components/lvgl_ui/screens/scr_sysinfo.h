#ifndef SCR_SYSINFO_H
#define SCR_SYSINFO_H

#include "lvgl.h"

extern lv_obj_t *ui_scrSysInfo;

void scr_sysinfo_prepare(void);
void scr_sysinfo_init(void);
void scr_sysinfo_uninit(void);
void scr_sysinfo_step(void);
void scr_sysinfo_reset_heap_tracker(void);

#endif
