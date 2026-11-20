#include "screens_update.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define UPDATE_QUEUE_DEPTH 16

static const char *TAG = "screens_update";

typedef struct {
    ui_update_cmd_t cmd;
    void           *data;
} ui_cmd_msg_t;

static QueueHandle_t s_queue = NULL;

/* ------------------------------------------------------------------ */
/* Handler table — one static function per ui_update_cmd_t value.
 * Add new handlers here and register them in s_handlers[].           */
/* ------------------------------------------------------------------ */

typedef void (*handler_fn_t)(void *data);

/* Placeholder — remove when UI_UPDATE_MAX has real commands */
#include "screens.h"
#include "scr_home.h"
#include "scr_sysinfo.h"
#include "scr_settings.h"

static void handle_btn_press(void *data)
{
    (void)data;
    ui_screens_on_activity();
    ui_screens_t cur = ui_screens_get_current();
    if (cur == SCREEN_HOME) {
        scr_home_step();
    } else if (cur == SCREEN_SYSINFO) {
        scr_sysinfo_reset_heap_tracker();
    } else if (cur == SCREEN_SETTINGS) {
        scr_settings_cycle_brightness();
    }
}

static const handler_fn_t s_handlers[UI_UPDATE_MAX] = {
    [UI_UPDATE_BTN_PRESS] = handle_btn_press,
};

/* ------------------------------------------------------------------ */

void ui_update_init(void)
{
    s_queue = xQueueCreate(UPDATE_QUEUE_DEPTH, sizeof(ui_cmd_msg_t));
    ESP_ERROR_CHECK(s_queue ? ESP_OK : ESP_FAIL);
}

void ui_update_cmd(ui_update_cmd_t cmd, void *data)
{
    if (!s_queue) return;
    ui_cmd_msg_t msg = { .cmd = cmd, .data = data };
    if (xQueueSend(s_queue, &msg, 0) != pdTRUE) {
        ESP_LOGW(TAG, "queue full, dropping cmd %d", (int)cmd);
    }
}

void ui_update_screens(void)
{
    if (!s_queue) return;
    ui_cmd_msg_t msg;
    while (xQueueReceive(s_queue, &msg, 0) == pdTRUE) {
        if (msg.cmd < UI_UPDATE_MAX && s_handlers[msg.cmd]) {
            s_handlers[msg.cmd](msg.data);
        }
    }
}
