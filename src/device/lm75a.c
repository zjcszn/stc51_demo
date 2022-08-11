#include "../inc/mcu/mcu.h"
#include "../inc/device/lm75a.h"
#include "../inc/device/lcd1602.h"
#include "../inc/protocol/i2c.h"
#include "../inc/protocol/uart.h"
#include <stdio.h>

#define LM75A_I2C_PORT      I2C_PORT_P3
#define LM75A_ADDR_WR       ((uint8_t)0X90)    // Adress Table:  1 0 0 1 A2 A1 A0 R1/W0
#define LM75A_ADDR_RD       ((uint8_t)0x91)    

#define LM75A_TEMP_REG      ((uint8_t)0x00)
#define LM75A_CONF_REG      ((uint8_t)0x01)
#define LM75A_THYST_REG     ((uint8_t)0x02)
#define LM75A_TOS_REG       ((uint8_t)0x03)

static void lm75a_show_int(uint8_t line, uint8_t column, int16_t num);
static void lm75a_show_deci(uint8_t line, uint8_t column, int16_t num);

void lm75a_read_temp(int16_t *temp_data){

    i2c_master_init(LM75A_I2C_PORT);
    i2c_master_start();

    //  发送 从机地址 并写入目标寄存器：温度寄存器 
    i2c_master_send_dat(LM75A_ADDR_WR);
    i2c_master_send_dat(LM75A_TEMP_REG);

    i2c_master_start();

    //  发送读指令 并读取高八位 及 低八位数据
    i2c_master_send_dat(LM75A_ADDR_RD);
    *temp_data = (int16_t)i2c_master_recive_dat();
    *temp_data <<=  8;
    i2c_master_send_ack();
    *temp_data |= (int16_t)i2c_master_recive_dat();
    i2c_master_send_noack();

    i2c_master_stop();

    //  处理数据，温度数据 D15 ~ D5,共 10 bits
    *temp_data &= 0xFFE0;
    *temp_data >>= 5;
}

void lm75a_show_temp(void) {

    int16_t read_buf;
    lm75a_read_temp(&read_buf);

    lcd1602_show_str(1, 1, "REAL TEMP       ");

    lm75a_show_int(2, 1, read_buf / 8);
    lcd1602_show_str(2, 4, ".");
    read_buf = read_buf < 0 ? -read_buf : read_buf;
    read_buf %= 8; 
    lm75a_show_deci(2, 5, (read_buf * 100) / 8);
    lcd1602_show_char(2, 8, 0x01);
    
}

void lm75a_show_uart(void) {
    xdata int16_t read_buf, i=0;
    xdata int16_t tmp_int;
    xdata int16_t tmp_deci;
    lm75a_read_temp(&read_buf);
    tmp_int = read_buf / 8;
    read_buf = (read_buf < 0) ? (-read_buf % 8) : (read_buf % 8);
    tmp_deci = (read_buf * 100) / 8;

    printf("REAL TEMP : %02d.%02d\n", tmp_int, tmp_deci);
}

static void lm75a_show_int(uint8_t line, uint8_t column, int16_t num) {
    uint8_t sign = num < 0;
    if (sign) {num = -num;}
    column += 2;
    do {
        lcd1602_show_char(line, column--, '0' + (num % 10));
        num /= 10;
    } while (num);
    if (sign) {lcd1602_show_char(line, column--, '-');}
    while (column) {
        lcd1602_show_char(line, column--, ' ');
    }
}

static void lm75a_show_deci(uint8_t line, uint8_t column, int16_t num) {
    if(num < 10) {lcd1602_show_char(line, column++, '0');}
    lcd1602_show_num(line, column, num);
}