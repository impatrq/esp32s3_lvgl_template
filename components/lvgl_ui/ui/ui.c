#include "ui.h"
#include "lvgl.h"

lv_obj_t *ui_scrSplash = NULL;
lv_obj_t *ui_scrHome   = NULL;
lv_obj_t *ui_scrMain   = NULL;

void ui_init(void)
{
    ui_scrSplash = lv_obj_create(NULL);
    ui_scrHome   = lv_obj_create(NULL);
    ui_scrMain   = lv_obj_create(NULL);
}
