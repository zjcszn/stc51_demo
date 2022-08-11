#include "../inc/framework/menu/alarm_set.h"
#include "../inc/framework/menu/clock_set.h"
#include "../inc/framework/menu.h"
#include "../inc/framework/task.h"
#include "../inc/framework/keyscan.h"
#include "../inc/device/ds1302.h"
#include "../inc/device/beep.h"
#include "../inc/device/lcd1602.h"
#include "../inc/device/eeprom.h"
#include "../inc/mcu/delay.h"
#include <stdio.h>

#define ALARM_BEEP_DELAY            ((uint8_t)0x50)

#define ALARM_DATA_ADDR         ((uint8_t)0x00)

static xdata uint8_t alarm_trig_flag = 0;
static xdata uint8_t alarm_en_flag = 0;
static xdata ds1302_clockdata alarm_set_data = {0, 1, 1, 0, 0, 0, 7};       // 初始 2000年 1月 1日 0时 0分 0秒 周六

static xdata uint8_t alarm_switch_str[2][4] = {"OFF", "ON "};

void alarm_write(uint8_t alarm_en_setval);

void alarm_set(void) {
    static xdata uint8_t reload_flag = 1;
    static xdata uint8_t cur_index = 0;
    static xdata uint8_t blink_init_flag = 1;
    static xdata uint8_t alarm_en_setval;
    
    if (reload_flag) {
        reload_flag = 0; 
        cur_index = 0;
        blink_init_flag = 1;
        alarm_en_setval = alarm_en_flag;
    }
    
    ds1302_show_clockdata(&alarm_set_data);
    lcd1602_show_str(2, 13, alarm_switch_str[alarm_en_setval]);
    clock_set_blink(cur_index, &blink_init_flag);

    switch (key_events_read()) {
        case 0x02:
            alarm_write(alarm_en_setval);
        case 0x01:
            reload_flag = 1;
            lcd1602_clear();
            task_sch_del(get_current_task_id());
            task_sch_add(&main_menu, 0, MENU_TASK_DELAY);
            break;

        case 0x08:
            cur_index = ++cur_index % 6;
            break;

        case 0x04:
        case 0x40:
            clock_data_update(&alarm_set_data, cur_index, VALUE_ADD);
            break;

        case 0x84:
            clock_data_update(&alarm_set_data, cur_index, VALUE_DEC);
            break;
        
        case 0x82:
            alarm_en_setval = !alarm_en_setval;
            break;

        default:
            break;
    }
}


void alarm_trig_check(void) {
    data uint8_t i;
    data ds1302_clockdata real_time_buf;
    get_ds1302_rtc_data(&real_time_buf);

    if (alarm_en_flag == 1 && alarm_trig_flag == 0){
        alarm_trig_flag = 1;

        for (i = 0; i < 7; i++) {
            if (((uint8_t *)&real_time_buf)[i] != ((uint8_t *)&alarm_set_data)[i]) {
                alarm_trig_flag = 0;
                break;
            }
        }        
        
        if (alarm_trig_flag == 1) {
            task_sch_add(&beep, 0, ALARM_BEEP_DELAY);
        }
    }

}

uint8_t get_alarm_en_status(void) {
    return alarm_en_flag;
}

uint8_t get_alarm_trig_status(void) {
    return alarm_trig_flag;
}

void alarm_trig_clear(void) {
    alarm_trig_flag = 0;
}

/**
 * @brief MCU上电初始化时从 M24C16 读取闹钟配置信息
 * 
 */
void alarm_read_init(void) {              
    data uint8_t alarm_data_head;
    data uint8_t *p_alarm_set_data = (uint8_t *)&alarm_set_data;

    eeprom_read(ALARM_DATA_ADDR, &alarm_data_head, 1);      // 读取帧头，如果帧头为0xAF, 则继续读取配置信息
    if (alarm_data_head == 0xAF) {
        eeprom_read(ALARM_DATA_ADDR + 1, p_alarm_set_data, 7);     // 读取闹钟日期及时间数据
    }
    eeprom_read(ALARM_DATA_ADDR + 8, &alarm_en_flag, 1);    // 读取闹钟功能开关标志位
}


/**
 * @brief 将闹钟配置信息写入 M24C16, 写入间隔应大于5ms
 * 
 * @param alarm_en_setval 
 */
void alarm_write(uint8_t alarm_en_setval) {
    data uint8_t i;
    data uint8_t *p_alarm_set_data = (uint8_t *)&alarm_set_data;
    idata uint8_t write_buf[16] = 0x00;

    write_buf[0] = 0xAF;                // 将闹钟配置数据及帧头写入发送缓冲区
    for (i= 0; i <= 6; i++) {
        write_buf[i + 1] = p_alarm_set_data[i];
    }

    alarm_en_flag = alarm_en_setval;
    write_buf[i + 1] = alarm_en_flag;

    eeprom_write(ALARM_DATA_ADDR, write_buf, 16);       // 将发送缓冲器数据写入EEPROM
}
