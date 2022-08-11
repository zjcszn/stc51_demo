#ifndef __74HC573_H__
#define __74HC573_H__

#include "../inc/mcu/type.h"

#define OUTPUT_NULL             0x10    // &= 0001 1111     锁存
#define OUTPUT_LED              0x20    // |= 0010 0000     LED输出
#define OUTPUT_7SEG_SEG         0xE0    // |= 1110 0000     动态数码管段选输出
#define OUTPUT_7SEG_BIT         0x60    // |= 1100 0000     动态数码管位选输出
#define OUTPUT_RESERVE          0xA0    // |= 1010 0000     备用输出

#define CS_PORT                 P0      // 片选端口  P05 : 74LS138 .C  P06 : 74LS138 .B  P07 : 74LS138 .A
#define DATA_PORT               P2      // 数据端口



/**
 * @brief 74HC573片选函数
 * 
 * @param chip_select 可选参数 
 *  OUTPUT_NULL | OUTPUT_LED | OUTPUT_7SEG_SEG | OUTPUT_7SEG_BIT | OUTPUT_RESERVE
 * @return void 
 */
void hc573_chip_select(uint8_t chip_select);

#endif