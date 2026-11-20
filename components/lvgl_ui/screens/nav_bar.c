#include "nav_bar.h"
#include "lvgl.h"

static lv_obj_t *s_bar = NULL;
static lv_obj_t *s_btns[3] = {NULL, NULL, NULL};
static const ui_screens_t s_targets[3] = {SCREEN_HOME, SCREEN_SYSINFO, SCREEN_SETTINGS};
static const char *s_labels[3] = {"Home", "SysInfo", "Settings"};

static void btn_event_cb(lv_event_t *e)
{
    ui_screens_on_activity();
    int idx = (int)(uintptr_t)lv_event_get_user_data(e);
    if (idx >= 0 && idx < 3) {
        ui_screens_navigate(s_targets[idx]);
    }
}

static void swipe_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    if (target && lv_obj_is_scrolling(target)) {
        return; /* Guard: ignore swipe during active scroll */
    }
    ui_screens_on_activity();

    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    ui_screens_t cur = ui_screens_get_current();

    if (dir == LV_DIR_LEFT) {
        if (cur == SCREEN_HOME) ui_screens_navigate(SCREEN_SYSINFO);
        else if (cur == SCREEN_SYSINFO) ui_screens_navigate(SCREEN_SETTINGS);
    } else if (dir == LV_DIR_RIGHT) {
        if (cur == SCREEN_SETTINGS) ui_screens_navigate(SCREEN_SYSINFO);
        else if (cur == SCREEN_SYSINFO) ui_screens_navigate(SCREEN_HOME);
    }
}

void nav_bar_attach_swipe(lv_obj_t *scr)
{
    if (!scr) return;
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(scr, swipe_event_cb, LV_EVENT_GESTURE, NULL);
}

void nav_bar_init(void)
{
    if (s_bar) return;

    s_bar = lv_obj_create(lv_layer_top());
    lv_obj_set_size(s_bar, lv_pct(100), 42);
    lv_obj_align(s_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(s_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(s_bar, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(s_bar, 2, 0);
    lv_obj_set_style_radius(s_bar, 0, 0);
    lv_obj_set_style_border_width(s_bar, 1, 0);
    lv_obj_set_style_border_side(s_bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_bg_color(s_bar, lv_color_hex(0x161B22), 0);
    lv_obj_set_style_border_color(s_bar, lv_color_hex(0x30363D), 0);
    lv_obj_clear_flag(s_bar, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < 3; i++) {
        s_btns[i] = lv_btn_create(s_bar);
        lv_obj_set_size(s_btns[i], lv_pct(31), 34);
        lv_obj_set_style_radius(s_btns[i], 6, 0);
        lv_obj_add_event_cb(s_btns[i], btn_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);

        lv_obj_t *lbl = lv_label_create(s_btns[i]);
        lv_label_set_text(lbl, s_labels[i]);
        lv_obj_center(lbl);
    }

    nav_bar_set_visible(false);
}

void nav_bar_set_visible(bool visible)
{
    if (!s_bar) return;
    if (visible) lv_obj_clear_flag(s_bar, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(s_bar, LV_OBJ_FLAG_HIDDEN);
}

void nav_bar_set_active(ui_screens_t screen)
{
    if (!s_bar) return;
    if (screen == SCREEN_SPLASH || screen >= SCREEN_MAX) {
        nav_bar_set_visible(false);
        return;
    }
    nav_bar_set_visible(true);
    for (int i = 0; i < 3; i++) {
        bool active = (s_targets[i] == screen);
        lv_obj_set_style_bg_color(s_btns[i],
            active ? lv_color_hex(0x2F81F7) : lv_color_hex(0x21262D), 0);
    }
}
