#ifndef __DS1302_H__
#define __DS1302_H__

#include "../inc/mcu/type.h"


#define VALUE_ADD               0
#define VALUE_DEC               1

//  年  月  日  星期  时  分  秒
typedef struct _ds1302_clockdata {
    uint8_t year;
    uint8_t month;     
    uint8_t day;          
    uint8_t hour;        
    uint8_t minute;      
    uint8_t second;
    uint8_t week;      
}ds1302_clockdata;


void ds1302_init(void);
void ds1302_show_rtc(void);
void ds1302_show_clockdata(const ds1302_clockdata * clock_data);
void ds1302_read_rtc(void);
void ds1302_write_rtc(const ds1302_clockdata *write_data);
void ds1302_show_rtc_7seg(void);
void ds1302_show_buffer(void);
void get_ds1302_rtc_data(ds1302_clockdata *clock_data_buf);
uint8_t ds1302_clockdata_check(const ds1302_clockdata *clock_data, const uint8_t index);
uint8_t ds1302_week_compute(int16_t year, int16_t month, int16_t day);
void ds1302_clockdata_init(ds1302_clockdata *clock_data, uint8_t index, uint8_t mode);
void clock_data_update(ds1302_clockdata *clock_data, const uint8_t index, const uint8_t update_mode);
void ds1302_clockdata_init(ds1302_clockdata *clock_data, uint8_t index, uint8_t init_mode);


#endif