#include "../inc/device/eeprom.h"
#include "../inc/protocol/i2c.h"

#define EEPROM_ADR          0xA0
#define EEPROM_I2C_PORT     I2C_PORT_P1

#define EEPROM_WR_BYTE      1
#define EEPROM_WR_PAGE      16



/**
 * @brief EEPROM 写入函数
 * 
 * @param write_adr 待写入的地址
 * @param write_data 待写入的数据
 * @param write_cnt 写入字节数  EEPROM_WR_BYTE | EEPROM_WR_PAGE
 */
void eeprom_write(uint8_t write_adr, uint8_t *write_data, uint8_t write_cnt) {
    i2c_master_init(EEPROM_I2C_PORT);
    i2c_master_start();

    if (i2c_master_send_dat(EEPROM_ADR)) {
        i2c_master_stop();
        return;
    }

    i2c_master_send_dat(write_adr);

    while (write_cnt --) {
    i2c_master_send_dat(*write_data++);
    }

    i2c_master_stop();
}


void eeprom_read(uint8_t read_adr, uint8_t *read_data, uint8_t read_cnt) {
    i2c_master_init(EEPROM_I2C_PORT);
    i2c_master_start();

    if (i2c_master_send_dat(EEPROM_ADR)) {
        i2c_master_stop();
        return;
    }

    i2c_master_send_dat(read_adr);
    i2c_master_start();
    i2c_master_send_dat(EEPROM_ADR | 0X01);

    *read_data++ = i2c_master_recive_dat();

    while (-- read_cnt) {
        i2c_master_send_ack();
        *read_data++ = i2c_master_recive_dat();
    }

    i2c_master_send_noack();
    i2c_master_stop();
}

