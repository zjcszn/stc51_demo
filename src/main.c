#include "../inc/protocol/i2c.h"
#include "../inc/device/lcd1602.h"
#include "../inc/mcu/mcu.h"
#include "../inc/framework/keyscan.h"
#include "../inc/device/ds1302.h"
#include "../inc/device/lm75a.h"
#include "../inc/framework/task.h"
#include "../inc/device/led.h"
#include "../inc/device/74hc573.h"
#include "../inc/device/7seg.h"
#include "../inc/protocol/uart.h"
#include "../inc/device/beep.h"
#include "../inc/framework/menu.h"
#include "../inc/device/eeprom.h"
#include "../inc/framework/menu/alarm_set.h"

xdata ds1302_clockdata tmp = {22, 12, 31, 10, 10, 0, 0};
uint8_t lcd1602_read_busy(void);
void main(void) {

    mcu_port_init();
    lcd1602_init();
    ds1302_init();
    timer_init();
    uart1_init();
    beep_pwm_init();

    ds1302_write_rtc(&tmp);
    ds1302_read_rtc();
    delay_1ms(1000);

    alarm_read_init();

    task_sch_add(&led_init, 100, 100);
    task_sch_add(&ds1302_read_rtc, 50, 50);
    task_sch_add(&main_display, 10, 10);
    task_sch_add(&alarm_trig_check, 70,70);

    while(1){
        task_sch_dispatch();
    }
}

