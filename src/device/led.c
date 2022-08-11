#include "../inc/device/74hc573.h"
#include "../inc/device/led.h"
#include "../inc/mcu/mcu.h"


void led_init(void) {

    static idata uint8_t led_value = 0x01;
    static idata uint8_t led_seq_flag = 0;

    if (led_value == 0x01) {led_seq_flag = 0;}
    if (led_value == 0x80) {led_seq_flag = 1;}

    hc573_chip_select(OUTPUT_LED);
    DATA_PORT = led_value;

    if (led_seq_flag) {
        led_value >>= 1;
    }
    else {
        led_value <<= 1;
    }
    hc573_chip_select(OUTPUT_NULL);

}