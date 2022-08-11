#include "../inc/protocol/uart.h"
#include "../inc/mcu/mcu.h"

#define UART_BUFFER_SIZE       512

static bit uart1_busy;
static uint16_t wrptr;
static uint16_t rdptr;

static xdata uint8_t uart1_buffer[UART_BUFFER_SIZE];

void uart1_init(void) {

    P30 = 1;
    P31 = 1; 
    P_SW1 &= 0x3F;
	PCON &= 0x3F;
	SCON = 0x50;		//8位数据,可变波特率
    uart1_busy = 0;
    wrptr = 0;
    rdptr = 0;
    ES = 1;
    EA = 1;
}

void uart1_send(uint8_t dat) {

    while (uart1_busy);         // 判断必须在 SBUF赋值前， 否则会影响 LCD1602 RTC 显示， 怀疑会影响硬件I2C
    uart1_busy = 1;
    SBUF = dat;

}



void uart1_send_str(uint8_t *str) {
    while (*str) {
        uart1_send(*str++);
    }
}

void uart1_read(void) {
    while (rdptr != wrptr) {
        uart1_send(uart1_buffer[rdptr++]);
        rdptr &= 0x01FF; 
    }
}

void uart1_isr(void) interrupt 4 {
    if (TI) {
        TI = 0;
        uart1_busy = 0;
    }
    if (RI) {
        RI = 0;
        uart1_buffer[wrptr++] = SBUF;
        wrptr &= 0x01FF;
    }
}

char putchar(char c) {
    uart1_send(c);
    return c;
}
