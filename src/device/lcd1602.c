#include "../inc/device/lcd1602.h"
#include "../inc/protocol/i2c.h"
#include "../inc/mcu/delay.h"

#define LCD1602_I2C_PORT        I2C_PORT_P3

#define LCD1602_WR_CMD          ((uint8_t)0x00)   // RW=0 RS=0        写入指令寄存器 instruction rigister
#define LCD1602_WR_DAT          ((uint8_t)0x01)   // RW=0 RS=1        写入数据寄存器 data rigister
#define LCD1602_RD_BUSY_AC      ((uint8_t)0x02)   // RW=1 RS=0        读 busy flag & 及 address counter --- --> D7:busy flag  D6~D0:AC 
#define LCD1602_RD_DAT          ((uint8_t)0x03)   // RW=1 RS=1        读取数据寄存器

#define LCD1602_EN              ((uint8_t)0x04)   // LCD1602使能信号

#define CURSOR_SHOW_ON          ((uint8_t)0x0E)   // LCD1602 光标功能 打开
#define CURSOR_SHOW_OFF         ((uint8_t)0x0C)   // LCD1602 光标功能 关闭
#define CURSOR_BLINK_ON         ((uint8_t)0x0D)   // LCD1602 光标闪烁 打开
#define CURSOR_BLINK_OFF        ((uint8_t)0x0C)   // LCD1602 光标闪烁 关闭

#define LCD1602_CLEAR           ((uint8_t)0X01)   // LCD1602 清屏命令
#define LCD1602_HOME            ((uint8_t)0X02)   // LCD1602 返回 HOME 命令

#define LCD1602_SHIFT_DISPLAY   ((uint8_t)0x18)   // LCD1602 移动显示命令
#define LCD1602_SHIFT_CURSOR    ((uint8_t)0x10)   // LCD1602 移动光标命令
#define LCD1602_SHIFT_RIGHT     ((uint8_t)0x14)   // LCD1602 右移命令
#define LCD1602_SHIFT_LEFT      ((uint8_t)0x10)   // LCD1602 左移命令


static void lcd1602_set_pos(const uint8_t line, const uint8_t column);
static void lcd1602_write(const uint8_t write_cmd, const uint8_t send_data);
static void lcd1602_read(uint8_t read_cmd, uint8_t *rev_data);
static uint8_t lcd1602_read_busy(void);
static void lcd1602_set_custom_char(void);

static idata uint8_t  back_light_stat;

static xdata uint8_t lcd1602_cgram_addr[8] = {0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78};      // cgram地址

static xdata uint8_t lcd1602_custom_char[8][8] = {                                                  // 自定义字符字模
    {0x00,0x1F,0x0A,0x04,0x04,0x0A,0x1F,0x00},      // 闹钟字符
    {0x17,0x08,0x08,0x08,0x08,0x08,0x07,0x00},      // 摄氏度字符
    {0},
    {0},
    {0},
    {0},
    {0},
    {0}
};

void lcd1602_init(void) {    
    
    i2c_master_init(LCD1602_I2C_PORT);
    i2c_master_start();

    // LCD1602 上电初始化为 8 线模式
    i2c_master_send_dat(LCD1602_ADDR);
    i2c_master_send_dat(0x30);
    i2c_master_send_dat(0x30 | LCD1602_EN);
    i2c_master_send_dat(0x30);
    
    delay_1ms(1);
    i2c_master_send_dat(0x30);
    i2c_master_send_dat(0x30 | LCD1602_EN);
    i2c_master_send_dat(0x30);
    
    delay_1ms(1);
    i2c_master_send_dat(0x30);
    i2c_master_send_dat(0x30 | LCD1602_EN);
    i2c_master_send_dat(0x30);
    
    // LCD1602 设置 4 线模式
    delay_1ms(1);
    i2c_master_send_dat(0x20);
    i2c_master_send_dat(0x20 | LCD1602_EN);
    i2c_master_send_dat(0x20);

    i2c_master_stop();

    lcd1602_set_backlight(LIGHT_ON);          // 打开背光
    lcd1602_write(LCD1602_WR_CMD, 0X28);      // Function Set    :     4 bits data line  /  2 line  /  5*8 dots
    lcd1602_write(LCD1602_WR_CMD, 0X0C);      // Display Control :     display : on  /  cursor display : off  /  cursor blinking : off 
    lcd1602_write(LCD1602_WR_CMD, 0X06);      // Entry Mod Set   :     cursor move : increment  /  display shift : off
    lcd1602_write(LCD1602_WR_CMD, 0X01);      // Clear Display

    lcd1602_set_custom_char();                // 写入自定义字符
}

//  LCD1602 自定义字符函数
static void lcd1602_set_custom_char(void) {
    data uint8_t i, j;
    for (i = 0; i < 8; i++) {
        lcd1602_write(LCD1602_WR_CMD, lcd1602_cgram_addr[i]);
        for (j = 0; j < 8; j++) {
            lcd1602_write(LCD1602_WR_DAT, lcd1602_custom_char[i][j]);
        }
    }
}

//  获取 LCD1602 背光状态
uint8_t get_backlight_status(void) {
    return back_light_stat;
}

//  LCD1602 显示字符串数据函数
void lcd1602_show_str(const uint8_t line, const uint8_t column, char *str) {
    if (str == (void*)0) return;
    lcd1602_set_pos(line, column);
    while (*str) {
        lcd1602_write(LCD1602_WR_DAT, *str++);
    }
}

//  LCD1602 显示整型数据函数（最大32 bit 有符号整数）
void lcd1602_show_num(const uint8_t line, const uint8_t column, int32_t num) {
    int8_t i = 0;
    uint8_t write_buffer[12] = {0};

    lcd1602_set_pos(line, column);
    if (num < 0) {
        lcd1602_write(LCD1602_WR_DAT, '-');
        num = -num;
    }
    do {
        write_buffer[i++] = '0' + (num % 10);
        num /= 10;
    } while (num); 
    do {
        lcd1602_write(LCD1602_WR_DAT, write_buffer[--i]);
    } while (i >= 1);

}

//  LCD1602 读 DDGRAM 字符数据 
void lcd1602_read_char(const uint8_t line, const uint8_t column, uint8_t *read_data) {
    lcd1602_set_pos(line, column);
    lcd1602_read(LCD1602_RD_DAT, read_data);
}

//  LCD1602 显示单字符数据函数
void lcd1602_show_char(const uint8_t line, const uint8_t column, const uint8_t char_data) {
    lcd1602_set_pos(line, column);
    lcd1602_write(LCD1602_WR_DAT, char_data);
}

//  LCD1602 清屏函数
void lcd1602_clear(void) {
    lcd1602_write(LCD1602_WR_CMD, 0x01);
}

//  LCD1602 光标设置函数
void lcd1602_set_cursor(uint8_t show_mode, uint8_t blink_mode) {

    show_mode = show_mode ? CURSOR_SHOW_ON : CURSOR_SHOW_OFF;
    blink_mode = blink_mode ? CURSOR_BLINK_ON : CURSOR_BLINK_OFF;

    lcd1602_write(LCD1602_WR_CMD, show_mode | blink_mode);

}

//  LCD1602 光标/屏幕 移动函数
void lcd1602_shift(uint8_t shift_object, uint8_t shift_direction) {

    shift_object = shift_object ? LCD1602_SHIFT_DISPLAY : LCD1602_SHIFT_CURSOR;
    shift_direction = shift_direction ? LCD1602_SHIFT_RIGHT : LCD1602_SHIFT_LEFT;

    lcd1602_write(LCD1602_WR_CMD, shift_object | shift_direction);

}

//  LCD1602 背光设置函数
void lcd1602_set_backlight(const uint8_t set_value) {

    back_light_stat = set_value ? 0x08 : 0x00;
    lcd1602_write(LCD1602_WR_CMD, 0x00);

}


static void lcd1602_set_pos(const uint8_t line, const uint8_t column) {

    uint8_t ddram_addr;
    if(line == 0 || line > 2) return;
    if(column == 0 || column > 16) return;
    ddram_addr = 0x80 + (line - 1) * 0x40 + column - 1;
    lcd1602_write(LCD1602_WR_CMD, ddram_addr);

}

static void lcd1602_write(const uint8_t write_cmd, const uint8_t send_data) {
    
    uint8_t send_buf;

    i2c_master_init(LCD1602_I2C_PORT);
    i2c_master_start();

    send_buf = (send_data & 0xF0) | write_cmd | back_light_stat;        // 发送 高四位数据  D7 ~ D4
    i2c_master_send_dat(LCD1602_ADDR);
    i2c_master_send_dat(send_buf);    
    i2c_master_send_dat(send_buf | LCD1602_EN);    
    i2c_master_send_dat(send_buf);

    send_buf = (send_data << 4) | write_cmd | back_light_stat;          // 发送 低四位数据 D3 ~ D0
    i2c_master_send_dat(send_buf);    
    i2c_master_send_dat(send_buf | LCD1602_EN);    
    i2c_master_send_dat(send_buf);    

    i2c_master_stop();

    while (lcd1602_read_busy());
}

static void lcd1602_read(uint8_t read_cmd, uint8_t *rev_data) {

    i2c_master_init(LCD1602_I2C_PORT);
    read_cmd = read_cmd | 0xF0 | back_light_stat;       // 高四位置 1 以读取端口数据，低四位设置 读指令 及 LCD背光

    i2c_master_start();

    i2c_master_send_dat(LCD1602_ADDR);        // 发送从机地址
    i2c_master_send_dat(read_cmd);
    i2c_master_send_dat(read_cmd | LCD1602_EN);         // 写入读指令
    i2c_master_stop();

    i2c_master_start();
    i2c_master_send_dat(LCD1602_ADDR | 0x01);         // 发送从机地址 和 读信号
    *rev_data = i2c_master_recive_dat() & 0xF0;         // 读 D7 ~ D4 数据
    i2c_master_send_noack();
    i2c_master_stop();

    i2c_master_start();
    i2c_master_send_dat(LCD1602_ADDR);    
    i2c_master_send_dat(read_cmd);    
    i2c_master_send_dat(read_cmd | LCD1602_EN);
    i2c_master_stop();
    i2c_master_start();
    i2c_master_send_dat(LCD1602_ADDR | 0x01);    
    *rev_data |= i2c_master_recive_dat() >> 0x04;       // 读 D3 ~ D0 数据
    i2c_master_send_noack();

    i2c_master_stop();



}

//  LCD1602 忙标志确认函数
uint8_t lcd1602_read_busy(void) {

    uint8_t busy = 0;
    lcd1602_read(LCD1602_RD_BUSY_AC, &busy);        // 必须在LCD1602 初始化完成的情况下才可以 读忙标志，否则可能会死循环
    return (busy & 0x80);

}
