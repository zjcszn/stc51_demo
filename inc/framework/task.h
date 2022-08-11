#ifndef __TASK_H__
#define __TASK_H__

#include "../mcu/type.h"

uint8_t get_current_task_id(void);
void task_sch_update(void);
uint8_t task_sch_add(void (*p_func)(void), const uint16_t delay, uint16_t preiod);
void task_sch_dispatch(void);
void task_sch_del(uint8_t index);

#endif