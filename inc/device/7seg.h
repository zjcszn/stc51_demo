#ifndef __7SEG_H__
#define __7SEG_H__

#include "../mcu/type.h"


#define DOT_ON      0x80
#define DOT_OFF     0x00


/**
 * @brief 动态数码管显示函数
 * 
 * @param show_num 显示的数字
 * @param show_bit 显示的位数
 * @return void
 */
void led_7seg_show(uint8_t show_num, uint8_t show_bit);




#endif