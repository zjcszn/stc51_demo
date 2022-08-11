#include "../inc/protocol/i2c.h"
#include "../inc/mcu/mcu.h"
#include <intrins.h>

#define MSCMD_MASTER_IDLE                   0x80    // I2C 空闲命令
#define MSCMD_MASTER_START                  0x81    // I2C START命令
#define MSCMD_MASTER_SEND_DAT               0x82    // I2C 发送数据命令
#define MSCMD_MASTER_RECIVE_ACK             0x83    // I2C 主机接收 ACK 信号命令
#define MSCMD_MASTER_RECIVE_DAT             0x84    // I2C 主机接收数据命令
#define MSCMD_MASTER_SEND_ACK               0x85    // I2C 主机发送 ACK 信号命令
#define MSCMD_MASTER_STOP                   0x86    // I2C STOP 命令
#define MSCMD_MASTER_DATA_ACK               0x8A    // I2C 主机发送数据并接收 ACK 信号命令


static bit busy;        // 忙标志

void i2c_master_init(uint8_t port) {

 
    P_SW2  |= 0x80;

    switch(port) {
        case I2C_PORT_P1 :
            P_SW2 &= 0xCF; P1M0 &= 0xCF; P1M1 &= 0xCF; break;
        case I2C_PORT_P2 :
            P_SW2 &= 0xCF; P_SW2 |= 0x10; P2M0 &= 0xCF; P2M1 &= 0xCF; break;
        case I2C_PORT_P7 :
            P_SW2 &= 0xCF; P_SW2 |= 0x20; P7M0 &= 0x3F; P7M1 &= 0x3F; break;
        case I2C_PORT_P3 :
            P_SW2 &= 0xCF; P_SW2 |= 0x30; P3M0 &= 0xF3; P3M1 &= 0xF3; break;
        default:
            P_SW2 &= 0xCF; break;
    }

    I2CTXD  = 0x00;
    I2CRXD  = 0x00;
    I2CMSST = 0x00;      // 初始化 I2C 主机状态寄存器
    I2CCFG  = 0xCD;      // 打开 I2C 功能，并设置为主机模式，速率：400K(IRC:24MHZ)

    EA = 1;

    P_SW2 &= ~0x80;
}

void i2c_master_start(void) {
    P_SW2 |= 0x80;
    busy = 1;
    I2CMSCR = MSCMD_MASTER_START;
    while (busy);
}

uint8_t i2c_master_send_dat(uint8_t dat) {
    I2CTXD = dat;
    busy = 1;
    I2CMSCR = MSCMD_MASTER_DATA_ACK;
    while (busy);
    return (I2CMSST & 0x02);
}


uint8_t i2c_master_recive_dat(void) {
    busy = 1;
    I2CMSCR = MSCMD_MASTER_RECIVE_DAT;
    while (busy);
    return I2CRXD;
}

void i2c_master_send_ack(void) {
    I2CMSST = 0X00;
    busy = 1;
    I2CMSCR = MSCMD_MASTER_SEND_ACK;
    while (busy);
}

void i2c_master_send_noack(void) {
    I2CMSST = 0X01;
    busy = 1;
    I2CMSCR = MSCMD_MASTER_SEND_ACK;
    while (busy);
}

void i2c_master_stop(void) {
    busy = 1;
    I2CMSCR = MSCMD_MASTER_STOP;
    while (busy);
    P_SW2 &= ~0x80;
}

void i2c_isr() interrupt 24 {

    _push_(P_SW2);
    P_SW2 |= 0x80;
    if (I2CMSST & 0x40) {
        I2CMSST &= ~0x40;
        busy = 0;
    }
    _pop_(P_SW2);
}

