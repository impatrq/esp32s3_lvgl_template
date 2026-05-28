#include "scr_splash.h"
#include "screens.h"
#include "ui.h"
#include "lvgl.h"
#include "esp_log.h"
#include "bsp_display.h"

static const char *TAG = "scr_splash";

static lv_obj_t *lbl_title;
static lv_obj_t *lbl_dots;
static lv_timer_t *timer_dots;
static int dot_count = 0;

/* Animación de puntos suspensivos */
static void dots_timer_cb(lv_timer_t *t)
{
    dot_count = (dot_count + 1) % 4;
    switch (dot_count)
    {
    case 0:
        lv_label_set_text(lbl_dots, "Iniciando");
        break;
    case 1:
        lv_label_set_text(lbl_dots, "Iniciando.");
        break;
    case 2:
        lv_label_set_text(lbl_dots, "Iniciando..");
        break;
    case 3:
        lv_label_set_text(lbl_dots, "Iniciando...");
        break;
    }
}

/* Toque en cualquier punto → navegar a home */
static void splash_touch_cb(lv_event_t *e)
{
    ui_screens_navigate(SCREEN_HOME);
}

/* Fade-in de retroiluminación */
static void backlight_anim_cb(void *var, int32_t val)
{
    bsp_display_set_brightness(val);
}

void scr_splash_prepare(void)
{
    /* nada persistente que registrar */
}

void scr_splash_init(void)
{
    ESP_LOGD(TAG, "init");

    /* Fondo negro */
    lv_obj_set_style_bg_color(ui_scrSplash, lv_color_black(), 0);

    /* Título */
    lbl_title = lv_label_create(ui_scrSplash);
    lv_label_set_text(lbl_title, "ESP32-S3 App");
    lv_obj_set_style_text_color(lbl_title, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_20, 0);
    lv_obj_align(lbl_title, LV_ALIGN_CENTER, 0, -20);

    /* Label de puntos */
    lbl_dots = lv_label_create(ui_scrSplash);
    lv_label_set_text(lbl_dots, "Iniciando");
    lv_obj_set_style_text_color(lbl_dots, lv_color_white(), 0);
    lv_obj_align(lbl_dots, LV_ALIGN_CENTER, 0, 20);

    /* Timer de puntos suspensivos cada 500 ms */
    dot_count = 0;
    timer_dots = lv_timer_create(dots_timer_cb, 500, NULL);

    /* Handler de toque global */
    lv_obj_add_event_cb(ui_scrSplash, splash_touch_cb, LV_EVENT_CLICKED, NULL);

    /* Fade-in de retroiluminación */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, backlight_anim_cb);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 800);
    lv_anim_start(&a);
}

void scr_splash_uninit(void)
{
    ESP_LOGD(TAG, "uninit");
    if (timer_dots)
    {
        lv_timer_del(timer_dots);
        timer_dots = NULL;
    }
}

void scr_splash_step(void)
{
    /* nada por hacer cada tick */
}