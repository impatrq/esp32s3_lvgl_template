#include "scr_settings.h"
#include "nav_bar.h"
#include "screens_timeout.h"
#include "bsp_display.h"
#include <stdio.h>

lv_obj_t *ui_scrSettings = NULL;
static lv_obj_t *s_slider = NULL;
static lv_obj_t *s_lbl_bright = NULL;
static int s_brightness = 80;

static void apply_brightness(int val)
{
    if (val < 10) val = 10;
    if (val > 100) val = 100;
    s_brightness = val;
    bsp_display_set_brightness((uint8_t)s_brightness);

    if (s_lbl_bright) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Brightness: %d%%", s_brightness);
        lv_label_set_text(s_lbl_bright, buf);
    }
    if (s_slider && lv_slider_get_value(s_slider) != s_brightness) {
        lv_slider_set_value(s_slider, s_brightness, LV_ANIM_OFF);
    }
}

void scr_settings_cycle_brightness(void)
{
    int next = s_brightness + 25;
    if (next > 100) next = 25;
    apply_brightness(next);
}

static void slider_event_cb(lv_event_t *e)
{
    ui_screens_on_activity();
    lv_obj_t *slider = lv_event_get_target(e);
    apply_brightness((int)lv_slider_get_value(slider));
}

static void switch_event_cb(lv_event_t *e)
{
    ui_screens_on_activity();
    lv_obj_t *sw = lv_event_get_target(e);
    bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    disp_timeout_set_off_ms(checked ? 15000 : TIMEOUT_NEVER);
}

void scr_settings_prepare(void)
{
    ui_scrSettings = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_scrSettings, lv_color_hex(0x0D1117), 0);
    nav_bar_attach_swipe(ui_scrSettings);

    lv_obj_t *title = lv_label_create(ui_scrSettings);
    lv_label_set_text(title, "Settings");
    lv_obj_set_style_text_color(title, lv_color_hex(0x58A6FF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    s_lbl_bright = lv_label_create(ui_scrSettings);
    lv_obj_set_style_text_color(s_lbl_bright, lv_color_hex(0xC9D1D9), 0);
    lv_obj_align(s_lbl_bright, LV_ALIGN_TOP_LEFT, 20, 46);

    s_slider = lv_slider_create(ui_scrSettings);
    lv_slider_set_range(s_slider, 10, 100);
    lv_obj_set_width(s_slider, lv_pct(82));
    lv_obj_align(s_slider, LV_ALIGN_TOP_MID, 0, 74);
    lv_obj_add_event_cb(s_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *lbl_to = lv_label_create(ui_scrSettings);
    lv_label_set_text(lbl_to, "Screen Timeout (15s)");
    lv_obj_set_style_text_color(lbl_to, lv_color_hex(0xC9D1D9), 0);
    lv_obj_align(lbl_to, LV_ALIGN_TOP_LEFT, 20, 120);

    lv_obj_t *sw = lv_switch_create(ui_scrSettings);
    lv_obj_align(sw, LV_ALIGN_TOP_RIGHT, -20, 114);
    lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    apply_brightness(80);
}

void scr_settings_init(void) {}
void scr_settings_uninit(void) {}
void scr_settings_step(void) {}
