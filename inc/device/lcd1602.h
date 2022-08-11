#ifndef __LCD1602_H__
#define __LCD1602_H__

#include "../mcu/type.h"

/*************************************
 * PCF8574 <------> LCD1602 PIN TO PIN
 *
 * P7----->D7
 * P6----->D6
 * P5----->D5
 * P4----->D4
 * P3----->LCD1602_BACKLIGHT
 * P2----->LCD1602_EN
 * P1----->LCD1602_RW
 * P0----->LCD1602_RS
 *
***************************************/

#define LCD1602_ADDR            ((uint8_t)0X4E)


#define LIGHT_ON        1
#define LIGHT_OFF       0

#define CUR_SHOW_ON     1       
#define CUR_SHOW_OFF    0
#define CUR_BLK_ON      1     
#define CUR_BLK_OFF     0 

#define SHIFT_DISPLAY   1
#define SHIFT_CUR       0
#define SHIFT_RIGHT     1
#define SHIFT_LEFT      0



void lcd1602_init(void);
void lcd1602_set_backlight(const uint8_t set_value);
void lcd1602_show_str(const uint8_t line, const uint8_t column, const char *str);
void lcd1602_show_num(const uint8_t line, const uint8_t column, int32_t num);
void lcd1602_read_char(const uint8_t line, const uint8_t column, uint8_t *read_data);
void lcd1602_show_char(const uint8_t line, const uint8_t column, const uint8_t char_data);
void lcd1602_clear(void);
void lcd1602_set_cursor(uint8_t show_mode, uint8_t blink_mode);
void lcd1602_shift(uint8_t shift_object, uint8_t shift_direction);
uint8_t get_backlight_status(void);

#endif