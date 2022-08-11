#include "../inc/framework/menu/key_test.h"
#include "../inc/framework/task.h"
#include "../inc/device/lcd1602.h"
#include "../inc/framework/keyscan.h"
#include "../inc/framework/menu.h"

void key_test(void) {
    switch (key_events_read()) {
        case 0x01:
            lcd1602_show_str(2, 1, "PRS CLICK       "); break;
        case 0x02:
            lcd1602_show_str(2, 1, "STB CLICK       "); break;
        case 0x04:
            lcd1602_show_str(2, 1, "NUM CLICK       "); break;
        case 0x08:
            lcd1602_show_str(2, 1, "MOD CLICK       "); break;
        case 0x10:
            lcd1602_show_str(2, 1, "PRS LONG PRESS  "); break;
        case 0x20:
            lcd1602_show_str(2, 1, "STB LONG PRESS  "); break;
        case 0x40:
            lcd1602_show_str(2, 1, "NUM LONG PRESS  "); break;
        case 0x80:
            lcd1602_show_str(2, 1, "MOD LONG PRESS  "); break;
        case 0x81:
            key_events_read();
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(&main_menu, 0, MENU_TASK_DELAY);
            break;
        default:
            break;
    }
}