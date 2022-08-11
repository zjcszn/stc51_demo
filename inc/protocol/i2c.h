#ifndef __I2C_H__
#define __I2C_H__

#include "../mcu/type.h"

/***************************
 *  PORT    SCL     SDA
 *   P1     P1.5    P1.4
 *   P2     P2.5    P2.4
 *   P7     P7.7    P7.6
 *   P3     P3.2    P3.3
 * 
****************************/     

#define I2C_PORT_P1         0
#define I2C_PORT_P2         1
#define I2C_PORT_P3         2
#define I2C_PORT_P7         3


void i2c_master_init(uint8_t port);
void i2c_master_start(void);
uint8_t i2c_master_send_dat(uint8_t dat);
void i2c_master_send_ack(void);
void i2c_master_send_noack(void);
uint8_t i2c_master_recive_dat(void);
void i2c_master_stop(void);



#endif