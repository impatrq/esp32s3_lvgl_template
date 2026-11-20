#ifndef SCREENS_UPDATE_H
#define SCREENS_UPDATE_H

#include <stdint.h>

typedef enum {
    UI_UPDATE_BTN_PRESS = 0,
    UI_UPDATE_MAX
} ui_update_cmd_t;

void ui_update_init(void);
void ui_update_cmd(ui_update_cmd_t cmd, void *data);
void ui_update_screens(void);

#endif
