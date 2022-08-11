#include "../inc/device/74hc573.h"
#include "../inc/mcu/mcu.h"

void hc573_chip_select(uint8_t chip_select) {
    CS_PORT &= OUTPUT_NULL;
    DATA_PORT = 0x00;
    if (chip_select != OUTPUT_NULL) {
        CS_PORT |= chip_select;
    }
}