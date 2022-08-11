#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "../inc/mcu/type.h"

void eeprom_write(uint8_t write_adr, uint8_t *write_data, uint8_t write_cnt);
void eeprom_read(uint8_t read_adr, uint8_t *read_data, uint8_t read_cnt);

#endif