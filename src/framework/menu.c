#include "../inc/framework/menu.h"
#include "../inc/framework/keyscan.h"
#include "../inc/framework/task.h"

#include "../inc/device/ds1302.h"
#include "../inc/device/lcd1602.h"

#include "../inc/framework/menu/alarm_set.h"
#include "../inc/framework/menu/clock_set.h"
#include "../inc/framework/menu/key_test.h"
#include "../inc/framework/menu/light_set.h"
#include "../inc/framework/menu/temp_show.h"
#include "../inc/framework/menu/update_clock.h"

#define MENU_NAME_LENGTH            17
#define MENU_LIST_MAX_SIZE          7
#define MAIN_MENU_INDEX             (MENU_LIST_MAX_SIZE - 1)

#define MAIN_DISPLAY_DELAY          10


typedef struct _menu_struct {
    uint8_t menu_name[MENU_NAME_LENGTH];
    void(*menu_op)(void);
}menu_struct;

static xdata uint8_t alarm_flag_char[2] = {0x20, 0x00};

static xdata menu_struct menu_list[MENU_LIST_MAX_SIZE] = {
    {"1.CLOCK SET     ", &clock_set},
    {"2.ALARM SET     ", &alarm_set},
    {"3.UPDATE TIME   ", &update_clock},
    {"4.TEMP SHOW     ", &temp_show},
    {"5.LIGHT SET     ", &light_set},
    {"6.KEY TEST      ", &key_test},
    {"0.NULL          ", &main_menu}
};


void main_menu(void) {
    static uint8_t menu_index = 0;
    lcd1602_show_str(1, 1, "      MENU      ");
    lcd1602_show_str(2, 1, menu_list[menu_index].menu_name);

    switch (key_events_read()) {
        case 0x01:
            menu_index = 0;
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(&main_display, 0, MAIN_DISPLAY_DELAY);
            break;
        case 0x08:
            menu_index = ++menu_index % 6;
            break;
        case 0x02:
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(menu_list[menu_index].menu_op, 0, MENU_TASK_DELAY);
            break;
        default:
            break;
    }

}

void main_display(void) {

    if (get_alarm_trig_status()) {
        lcd1602_show_str(1, 1, "                ");
        lcd1602_show_str(2, 1, "  ALARM TRIG !  ");
    }
    else {
        ds1302_show_rtc();
        lcd1602_show_char(2, 14, alarm_flag_char[get_alarm_en_status()]);
    }

    
    switch (key_events_read()) {
        case 0x08:
            task_sch_del(get_current_task_id());
            task_sch_add(menu_list[MAIN_MENU_INDEX].menu_op, 0, MENU_TASK_DELAY);        // 跳转至主菜单
            lcd1602_clear();
        break;

        case 0x01:
            if (get_alarm_trig_status()) {
                alarm_trig_clear();
                lcd1602_clear();
            }
        default:
        break;
    }

}


