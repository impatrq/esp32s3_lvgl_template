#ifndef __SCREENS_TIMEOUT_H__
#define __SCREENS_TIMEOUT_H__

#include <stdint.h>
#include <stdbool.h>

/** Pass as a timeout value to disable that timeout entirely. */
#define TIMEOUT_NEVER  0

/**
 * Configure and enable the two-stage timeout:
 *
 *  timeout_to_main_ms   Navigate back to SCREEN_MAIN after this many
 *                       milliseconds of inactivity while on any other screen.
 *                       Set TIMEOUT_NEVER to disable.
 *
 *  timeout_to_off_ms    Turn the display off after this many milliseconds of
 *                       inactivity on the main screen (or any screen if
 *                       timeout_to_main_ms is TIMEOUT_NEVER).
 *                       Set TIMEOUT_NEVER to disable.
 *
 * Called once inside lvgl_ui_start(). You can update the off-timeout at
 * runtime (e.g. from a settings screen) with disp_timeout_set_off_ms().
 */
void disp_timeout_init(uint32_t timeout_to_main_ms, uint32_t timeout_to_off_ms);

/** Change the display-off timeout at runtime (thread-safe from LVGL task). */
void disp_timeout_set_off_ms(uint32_t timeout_ms);

/** Returns true if the display is currently on. */
bool disp_is_on(void);

/**
 * Tick function — evaluate inactivity timers and react.
 * Called every UI_STEP_PERIOD_MS by the LVGL timer in lvgl_ui.c.
 * Uses lv_disp_get_inactive_time() so no separate touch handler is needed.
 */
void disp_timeout_step(void);

#endif
