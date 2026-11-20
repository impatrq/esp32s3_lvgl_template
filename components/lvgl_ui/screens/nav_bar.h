#ifndef NAV_BAR_H
#define NAV_BAR_H

#include "screens.h"

void nav_bar_init(void);
void nav_bar_set_active(ui_screens_t screen);
void nav_bar_set_visible(bool visible);
void nav_bar_attach_swipe(lv_obj_t *scr);

#endif
