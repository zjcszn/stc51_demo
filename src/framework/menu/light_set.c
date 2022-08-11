#include "../inc/framework/menu/light_set.h"
#include "../inc/device/lcd1602.h"
#include "../inc/framework/task.h"
#include "../inc/framework/keyscan.h"
#include "../inc/framework/menu.h"

static xdata uint8_t light_set_str[2][17] = {"BACK LIGHT OFF  ", "BACK LIGHT ON   "};

void light_set(void) {
    static xdata uint8_t light_setval = 1;

    lcd1602_show_str(1, 1, "   LIGHT SET    ");
    lcd1602_show_str(2, 1, light_set_str[light_setval]);

    switch (key_events_read()) {

        case 0x02:
            lcd1602_set_backlight(light_setval);
        case 0x01:
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(&main_menu, 0, MENU_TASK_DELAY);
            break;

        case 0x08:
            light_setval = !light_setval;
            break;

        default:
            break;

    }
}
