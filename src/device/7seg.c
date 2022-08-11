#include "../inc/device/7seg.h"
#include "../inc/mcu/mcu.h"
#include "../inc/device/74hc573.h"


static xdata uint8_t DISPLAY_NUM[10]   =  {0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F};     // '0' '1' '2' '3' '4' '5' '6' '7' '8' '9'
static xdata uint8_t DISPLAY_BIT[8]    =  {0X7F, 0XBF, 0XDF, 0XEF, 0XF7, 0XFB, 0XFD, 0xFE};                 // bit1 bit2 bit3 bit4 bit6 bit6 bit7 bit8

void led_7seg_show(const uint8_t show_num, const uint8_t show_bit){


    hc573_chip_select(OUTPUT_7SEG_SEG);              // 段选输出使能
    DATA_PORT = DISPLAY_NUM[show_num];
    hc573_chip_select(OUTPUT_7SEG_BIT);              // 位选信号使能
    DATA_PORT = DISPLAY_BIT[show_bit - 1];
    hc573_chip_select(OUTPUT_NULL);                  // 锁存
}

