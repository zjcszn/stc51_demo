#ifndef __UART_H__
#define __UART_H__

#include "../inc/mcu/type.h"


void uart1_init(void);
void uart1_send(uint8_t dat);
void uart1_send_str(uint8_t *str);
void uart1_read(void);
char putchar(char c);


#endif