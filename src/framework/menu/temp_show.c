#include "../inc/framework/menu/temp_show.h"
#include "../inc/framework/keyscan.h"
#include "../inc/framework/task.h"
#include "../inc/framework/menu.h"
#include "../inc/device/lcd1602.h"
#include "../inc/device/lm75a.h"

void temp_show(void) {
    lm75a_show_temp();
    switch (key_events_read()) {
        case 0x01:
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(&main_menu, 0, MENU_TASK_DELAY);
            break;
        default:
            break;
    }

}