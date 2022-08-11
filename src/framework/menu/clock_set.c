#include "../inc/framework/menu/clock_set.h"
#include "../inc/mcu/type.h"
#include "../inc/device/ds1302.h"
#include "../inc/framework/keyscan.h"
#include "../inc/device/lcd1602.h"
#include "../inc/framework/task.h"
#include "../inc/framework/menu.h"




#define BLINK_DELAY             7

static xdata ds1302_clockdata clock_set_data;
static xdata clock_set_curlist[6][2] = {{1, 3}, {1, 6}, {1, 9}, {2, 1}, {2, 4}, {2, 7}};        // 年 月 日 时 分 秒



void clock_set(void) {
    static xdata uint8_t reload_flag = 1;
    static xdata uint8_t cur_index   = 0;
    static xdata uint8_t blink_init_flag  = 1;

    if (reload_flag) {
        get_ds1302_rtc_data(&clock_set_data);           // 函数重新加载时 获取当前时钟数据
        reload_flag = 0;
        cur_index = 0;
        blink_init_flag = 1;
    }


    ds1302_show_clockdata(&clock_set_data);             // 显示时钟设置数据
    clock_set_blink(cur_index, &blink_init_flag);            // 闪烁表示


    switch (key_events_read()) {
        case 0x02:                                      // "STB" 单击事件触发时，将数据写入 DS1302
            ds1302_write_rtc(&clock_set_data);          
        case 0x01:                                      // "PRS" 单击事件触发时，退出clock_set任务，并重置reload标志，加载main_menu任务
            reload_flag = 1;
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(&main_menu, 0, MENU_TASK_DELAY);
            break;

        case 0x08:                                      // "MOD" 单击事件触发时，更新 cur_index , 切换设置对象
            cur_index = ++cur_index % 6;
            blink_init_flag = 1;
            break;

        case 0x04:                                      // "NUM" 单击事件及长按事件触发时， 所设置的对象数值 + 1
        case 0x40:
            clock_data_update(&clock_set_data, cur_index, VALUE_ADD);
            break;

        case 0x84:                                      // "MOD + NUM" 组合按键事件触发时， 所设置的对象数值 - 1
            clock_data_update(&clock_set_data, cur_index, VALUE_DEC);
            break;

        default:
            break;
    }
}


/**
 * @brief 时钟设置界面 光标闪烁函数
 * 
 * @param line 行号
 * @param column 列号
 */
void clock_set_blink(const uint8_t cur_index, uint8_t *blink_init_flag) {
    static xdata uint8_t delay_cnt = 0;
    static xdata uint8_t blink_flag = 1;

    if (cur_index >= 6) return; 
    
    if (*blink_init_flag) {         // 初始化
        delay_cnt = 0;
        blink_flag = 1;
        *blink_init_flag = 0;
    }

    delay_cnt = ++delay_cnt % BLINK_DELAY;
    if (!delay_cnt) {
        blink_flag = !blink_flag;
    }

    if (blink_flag) {
        lcd1602_show_str(clock_set_curlist[cur_index][0], clock_set_curlist[cur_index][1], "  ");
    }
}


