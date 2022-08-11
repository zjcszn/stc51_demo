#ifndef __ALARM_SET_H__
#define __ALARM_SET_H__

#include "../../mcu/type.h"


void alarm_read_init(void);
void alarm_set(void);
void alarm_trig_check(void);
uint8_t get_alarm_en_status(void);
uint8_t get_alarm_trig_status(void);
void alarm_trig_clear(void);
void alarm_write(uint8_t alarm_en_setval);

#endif