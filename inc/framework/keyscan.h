#ifndef __KEYSCAN_H__
#define __KEYSCAN_H__

#include "../inc/mcu/type.h"



void key_scan(void);
uint8_t key_events_read(void);

#endif