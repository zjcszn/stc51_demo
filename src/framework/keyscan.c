#include "../inc/framework/keyscan.h"
#include "../inc/mcu/mcu.h"
#include "../inc/device/lcd1602.h"


#define COMMON_PORT     P17
#define KEY_PORT        P1          // KEY_PORT.0 KEY_PORT.1 KEY_PORT.2 KEY_PORT.3

#define KEY_MAX_SIZE    4

#define KEY_MOD         0
#define KEY_NUM         1
#define KEY_STB         2
#define KEY_PRS         3


#define KEY_RELEASE_STABLE      ((uint8_t)0x00)             // 按键状态：释放稳定
#define KEY_PRESS_SHAKE         ((uint8_t)0x01)             // 按键状态：按下抖动
#define KEY_PRESS_STABLE        ((uint8_t)0x02)             // 按键状态：按下稳定
#define KEY_RELEASE_SHAKE       ((uint8_t)0x03)             // 按键状态：释放抖动

#define KEY_NULL_EVENT      0               // 空事件
#define KEY_LONG_PRESS      1               // 长按事件
#define KEY_SINGLE_CLICK    1               // 单击事件


typedef struct _key_struct {
    uint8_t key_index : 4;
    uint8_t key_state : 4;
    uint8_t key_cnt;
}key_struct;

static key_struct g_key_struct[KEY_MAX_SIZE] = {
    {KEY_MOD, KEY_RELEASE_STABLE, 0},
    {KEY_NUM, KEY_RELEASE_STABLE, 0},
    {KEY_STB, KEY_RELEASE_STABLE, 0},
    {KEY_PRS, KEY_RELEASE_STABLE, 0},
};


/****************************************
 * @ key_events
 * 
 * key_long_press_mod       bit 7
 * key_long_press_num       bit 6
 * key_long_press_stb       bit 5
 * key_long_press_prs       bit 4
 * key_single_click_mod     bit 3
 * key_single_click_num     bit 2
 * key_single_click_stb     bit 1
 * key_single_click_prs     bit 0
*****************************************/
static uint8_t g_key_events = 0;


void key_scan(void) {
    uint8_t i;
    uint8_t key_value;

    COMMON_PORT = 0;
    KEY_PORT |= 0x0F;

    // 按键扫描
    for (i = 0; i <= 3; i++) {      
        key_value = !(KEY_PORT & (0x01 << i));

        switch (g_key_struct[i].key_state) {

            case KEY_RELEASE_STABLE:
                // “释放稳定” 状态下，当 “按下按键” 事件发生，则跳转到 “按下抖动” 状态
                if (key_value) {             
                    g_key_struct[i].key_state = KEY_PRESS_SHAKE;
                }
                break;

            case KEY_PRESS_SHAKE:
                // “按下抖动” 状态下，当持续 3 个周期检测到 “按键按下”， 则跳转到 “按下稳定” 状态
                if (key_value) {
                    if (++g_key_struct[i].key_cnt == 3) {
                        g_key_struct[i].key_state = KEY_PRESS_STABLE;
                        g_key_struct[i].key_cnt = 0;
                    }
                }
                // “按下抖动“ 状态下，当检测到 “按键释放”， 则跳转回上个状态
                else {
                    g_key_struct[i].key_state = KEY_RELEASE_STABLE;
                }
                break;

            case KEY_PRESS_STABLE:
                // “按下稳定” 状态下，当持续 50 个周期检测到 “按键按下”， 则触发 长按事件；
                if (key_value) {
                    if (++g_key_struct[i].key_cnt == 20) {
                        g_key_events |= (0x80 >> i);
                        g_key_struct[i].key_cnt = 0; 
                    }
                }
                // “按下稳定” 状态下，当检测到 “按键释放” ，则跳转到下个状态
                else {
                    g_key_struct[i].key_state = KEY_RELEASE_SHAKE;
                    g_key_struct[i].key_cnt = 0;
                }
                break;

            case KEY_RELEASE_SHAKE:
                // “释放抖动” 状态下，当检测到 “按键按下”，则跳转回上个状态
                if (key_value) {
                    g_key_struct[i].key_state = KEY_PRESS_STABLE;
                    g_key_struct[i].key_cnt = 0;
                }
                // “释放抖动” 状态下，当持续 3 个周期检测到 “按键释放”， 则跳转到 “释放稳定” 状态，并触发 单击事件
                else {
                    if (++g_key_struct[i].key_cnt == 3) {
                        g_key_struct[i].key_state = KEY_RELEASE_STABLE;
                        g_key_struct[i].key_cnt = 0;

                        if (g_key_events & (0x80 >> i)) {
                            g_key_events &= ~(0x80 >> i);
                        }
                        else {
                            g_key_events |= (0x08 >> i);
                        }


                    }
                }
                break;

            default:
                break;
        }
    
    }

}

//  读取按键事件并清除

uint8_t key_events_read(void) {
    uint8_t tmp = g_key_events;
    g_key_events &= 0xF0;
    return tmp;
}
