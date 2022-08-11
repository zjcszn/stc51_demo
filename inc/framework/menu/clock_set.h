#ifndef __CLOCK_SET_H__
#define __CLOCK_SET_H__

#include "../../mcu/type.h"

void clock_set(void);
void clock_set_blink(const uint8_t cur_index, uint8_t *blink_init_flag);


#endif