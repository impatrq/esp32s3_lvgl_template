#include "scr_main.h"
#include "screens.h"
#include "ui.h"             /* access Squareline widget objects (e.g. ui_lblTitle) */
#include "esp_log.h"

static const char *TAG = "scr_main";

/* ------------------------------------------------------------------ */
/* Persistent event callbacks — registered in prepare(), survive screen
 * transitions because they are attached to the widget, not the screen. */
/* ------------------------------------------------------------------ */

/* Example:
static void on_btn_pressed(lv_event_t *e)
{
    (void)e;
    ui_screens_navigate(SCREEN_SETTINGS);
}
*/

/* ------------------------------------------------------------------ */

void scr_main_prepare(void)
{
    /* Register callbacks on widgets that must stay active across screens.
     * Example:
     *   lv_obj_add_event_cb(ui_btnSettings, on_btn_pressed, LV_EVENT_CLICKED, NULL);
     */
    ESP_LOGD(TAG, "prepare");
}

void scr_main_init(void)
{
    /* Called every time the main screen becomes active.
     * Refresh dynamic content, restart animations, etc.
     * Example:
     *   lv_label_set_text(ui_lblStatus, "Ready");
     */
    ESP_LOGD(TAG, "init");
}

void scr_main_uninit(void)
{
    /* Called when leaving the main screen.
     * Stop any screen-local timers or animations here.
     */
    ESP_LOGD(TAG, "uninit");
}

void scr_main_step(void)
{
    /* Called every ~20 ms while the main screen is active.
     * Poll sensors, update progress bars, handle encoder deltas, etc.
     * Keep this function fast — avoid blocking calls.
     */
}
