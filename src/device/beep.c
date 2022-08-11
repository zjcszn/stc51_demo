#include "../inc/device/beep.h"
#include "../inc/mcu/mcu.h"
#include "../inc/framework/task.h"
#include "../inc/framework/menu/alarm_set.h"
#include <intrins.h>


/******************************
 * PWM0
*******************************/

#define PWM0    0
#define PWM1    1
#define PWM2    2
#define PWM3    3
#define PWM4    4
#define PWM5    5
#define PWM6    6
#define PWM7    7

#define PWM_PORT_P2 0
#define PWM_PORT_P1 1
#define PWM_PORT_P6 2

extern uint8_t alarm_trig_flag;

void beep_pwm_init(void) {
    _push_(P_SW2);

    P1M1 &= 0xBF; 
    P1M0 |= 0x40;       // P1.6 推挽输出 
    PWMSET = 0X01;
    P_SW2 |= 0x80;

    PWMCKS = 0x00;
    PWMC = 0X1000;
    PWM6T1 = 0x0000;
    PWM6T2 = 0x0800;
    PWM6CR = 0x88;
    _pop_(P_SW2);
}


void beep(void) {
    static idata uint8_t beep_flag = 0;
    PWMCFG = beep_flag ? 0x01 : 0x00;
    beep_flag = ++beep_flag % 2;
    if (!get_alarm_trig_status()) {
        PWMCFG = 0X00;
        task_sch_del(get_current_task_id());
    }
}

