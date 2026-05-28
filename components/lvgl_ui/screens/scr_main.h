#ifndef __SCR_MAIN_H__
#define __SCR_MAIN_H__

/**
 * Main screen controller.
 *
 * Convention for all screen controllers:
 *   prepare()  — called once at startup; register persistent event callbacks
 *                on widgets that should survive screen transitions.
 *   init()     — called each time this screen becomes active; restore state,
 *                start animations, refresh dynamic labels.
 *   uninit()   — called when navigating away; stop timers, free temporaries.
 *   step()     — called every ~20 ms while active; poll sensors, drive
 *                incremental animations, handle encoder input.
 *
 * All four are optional (registered as NULL in screens.c skips the call).
 */
void scr_main_prepare(void);
void scr_main_init(void);
void scr_main_uninit(void);
void scr_main_step(void);

#endif
