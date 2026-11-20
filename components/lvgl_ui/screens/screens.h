#ifndef SCREENS_H
#define SCREENS_H

#include "lvgl.h"

typedef struct {
    const char *name;
    lv_obj_t  **screen;
    void (*prepare)(void);
    void (*init)(void);
    void (*uninit)(void);
    void (*step)(void);
} screen_config_t;

#define SCREEN_REGISTER(name, scr, fn_name) \
    { name, &scr, scr_##fn_name##_prepare, scr_##fn_name##_init, scr_##fn_name##_uninit, scr_##fn_name##_step }

typedef enum {
    SCREEN_SPLASH = 0,
    SCREEN_HOME,
    SCREEN_SYSINFO,
    SCREEN_SETTINGS,
    SCREEN_MAX
} ui_screens_t;

void         ui_screens_prepare(void);
void         ui_screens_step(void);
ui_screens_t ui_screens_get_current(void);
void         ui_screens_navigate(ui_screens_t screen);
void         ui_screens_on_activity(void);

#endif
