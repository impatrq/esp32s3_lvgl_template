#ifndef __SCREENS_H__
#define __SCREENS_H__

#include "lvgl.h"

#define SCREEN_REGISTER(name, scr, fn_name) { name, &scr, scr_##fn_name##_prepare, scr_##fn_name##_init, scr_##fn_name##_uninit, scr_##fn_name##_step }

/**
 * Screen enum — add a value for every screen in your Squareline project,
 * in the order you want them registered. Keep SCREEN_MAX last.
 */
typedef enum {
    SCREEN_MAIN = 0,
    /* SCREEN_SETTINGS, */
    /* SCREEN_ERROR,    */
    SCREEN_MAX
} ui_screens_t;

/**
 * Per-screen descriptor. Fill one entry per screen in screen_configs[] inside
 * screens.c. All callback pointers are optional (may be NULL).
 *
 *  screen_obj  Pointer to the Squareline lv_obj_t* for this screen
 *              (e.g. &ui_Screen1, &ui_scrSettings …).
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

/**
 * Main tick function — detect screen transitions and drive lifecycle callbacks.
 * Called every UI_STEP_PERIOD_MS by the LVGL timer in lvgl_ui.c.
 */
void          ui_screens_step(void);

/** Returns the enum value matching the currently active LVGL screen. */
ui_screens_t  ui_screens_get_current(void);

/**
 * Navigate to a screen programmatically. Equivalent to lv_disp_load_scr()
 * but uses the registry, so the lifecycle engine tracks the transition.
 */
void          ui_screens_navigate(ui_screens_t screen);

/**
 * Signal user activity (touch, encoder …). Resets the LVGL inactivity timer
 * used by screens_timeout to decide when to dim / turn off the display.
 */
void          ui_screens_on_activity(void);

#endif
