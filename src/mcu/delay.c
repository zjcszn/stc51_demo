#include "../inc/mcu/delay.h"
#include <intrins.h>



// @ 24MHZ
void delay_1ms(uint16_t delay_time) {
	unsigned char i, j;

	do {
        _nop_();
        i = 32;
	    j = 40;
        do {
            while (--j);
        } while (--i);
    } while (--delay_time);
}

void delay_1us(uint16_t delay_time) {
	unsigned char i;

    do {
	    i = 6;
	    while (--i);
    } while (--delay_time);
}
