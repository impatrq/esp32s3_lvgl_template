#ifndef __SCREENS_H__
#define __SCREENS_H__

#include "lvgl.h"

#define SCREEN_REGISTER(name, scr, fn_name) { name, &scr, scr_##fn_name##_prepare, scr_##fn_name##_init, scr_##fn_name##_uninit, scr_##fn_name##_step }

/**
 * Screen enum — one value per screen of the Squareline project,
 * in the order they are registered. Keep SCREEN_MAX last.
 */
typedef enum {
    SCREEN_SPLASH = 0,
    SCREEN_HOME,
    SCREEN_MAX
} ui_screens_t;

/**
 * Per-screen descriptor. One entry per screen in screen_configs[] (screens.c).
 *
 *  screen_obj  Pointer to the Squareline lv_obj_t* for this screen.
 *  prepare     Called once at startup — register persistent event callbacks.
 *  init        Called each time this screen becomes the active one.
 *  uninit      Called when navigating away from this screen.
 *  step        Called every UI tick (~20 ms) while this screen is active.
 */
typedef struct {
    const char  *name;
    lv_obj_t   **screen_obj;
    void        (*prepare)(void);
    void        (*init)(void);
    void        (*uninit)(void);
    void        (*step)(void);
} screen_config_t;

/** Call prepare() on every registered screen. Called once inside lvgl_ui_start(). */
void          ui_screens_prepare(void);

/** Main tick function — detect screen transitions and drive lifecycle callbacks. */
void          ui_screens_step(void);

/** Returns the enum value matching the currently active LVGL screen. */
ui_screens_t  ui_screens_get_current(void);

/** Navigate to a screen programmatically, tracked by the lifecycle engine. */
void          ui_screens_navigate(ui_screens_t screen);

/** Signal user activity (touch, encoder …). Resets the inactivity timer. */
void          ui_screens_on_activity(void);

#endif