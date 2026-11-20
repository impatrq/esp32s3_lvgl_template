#include "scr_splash.h"
#include "screens.h"
#include "ui.h"
#include "esp_log.h"

static const char *TAG = "scr_splash";

static lv_timer_t *s_dots_timer = NULL;
static uint8_t s_dots = 0;

/* Cycles "Iniciando" -> "Iniciando." -> "Iniciando.." -> "Iniciando..." */
static void on_dots_timer(lv_timer_t *t)
{
    (void)t;
    static const char *frames[] = { "Iniciando", "Iniciando.", "Iniciando..", "Iniciando..." };
    s_dots = (s_dots + 1) % 4;
    lv_label_set_text(ui_lblLoading, frames[s_dots]);
}

/* Touch anywhere on the splash -> go to home */
static void on_splash_clicked(lv_event_t *e)
{
    (void)e;
    ui_screens_navigate(SCREEN_HOME);
}

void scr_splash_prepare(void)
{
    /* The screen itself must be clickable to receive a tap anywhere */
    lv_obj_add_flag(ui_scrSplash, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrSplash, on_splash_clicked, LV_EVENT_CLICKED, NULL);
    ESP_LOGD(TAG, "prepare");
}

void scr_splash_init(void)
{
    s_dots = 0;
    lv_label_set_text(ui_lblLoading, "Iniciando");
    s_dots_timer = lv_timer_create(on_dots_timer, 400, NULL);
}

void scr_splash_uninit(void)
{
    if (s_dots_timer) {
        lv_timer_del(s_dots_timer);
        s_dots_timer = NULL;
    }
}

void scr_splash_step(void)
{
}