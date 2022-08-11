#include "../inc/device/ds1302.h"
#include "../inc/mcu/mcu.h"
#include "../inc/mcu/delay.h"
#include "../inc/device/lcd1602.h"
#include "../inc/device/7seg.h"

sbit DS1302_SCLK = P3^5;
sbit DS1302_IO   = P3^6;
sbit DS1302_CE   = P3^7;


#define DS1302_WR_ADDR_SEC              ((uint8_t)0x80)
#define DS1302_RD_ADDR_SEC              ((uint8_t)0x81)
#define DS1302_WR_ADDR_MIN              ((uint8_t)0x82)
#define DS1302_RD_ADDR_MIN              ((uint8_t)0x83)
#define DS1302_WR_ADDR_HOUR             ((uint8_t)0x84)
#define DS1302_RD_ADDR_HOUR             ((uint8_t)0x85)
#define DS1302_WR_ADDR_DAY              ((uint8_t)0x86)
#define DS1302_RD_ADDR_DAY              ((uint8_t)0x87)
#define DS1302_WR_ADDR_MON              ((uint8_t)0x88)
#define DS1302_RD_ADDR_MON              ((uint8_t)0x89)
#define DS1302_WR_ADDR_WEEK             ((uint8_t)0x8A)
#define DS1302_RD_ADDR_WEEK             ((uint8_t)0x8B)
#define DS1302_WR_ADDR_YEAR             ((uint8_t)0x8C)
#define DS1302_RD_ADDR_YEAR             ((uint8_t)0x8D)
#define DS1302_WR_ADDR_WP               ((uint8_t)0x8E)
#define DS1302_RD_ADDR_WP               ((uint8_t)0x8F)

#define DS1302_SPI_DELAY                1           // SPI 延迟


#define INDEX_YEAR                      0
#define INDEX_MON                       1
#define INDEX_DAY                       2
#define INDEX_HOUR                      3
#define INDEX_MIN                       4
#define INDEX_SEC                       5
#define INDEX_WEEK                      6

#define LIMIT_MIN                       0
#define LIMIT_MAX                       1
#define INIT_MIN                        0
#define INIT_MAX                        1

static void lcd1602_show_num_override(uint8_t line, uint8_t column, const uint16_t num);
static void ds1302_spi_write(const uint8_t write_addr, const uint8_t write_data);
static void ds1302_spi_read(const uint8_t read_addr, uint8_t * const read_data);
static uint8_t ds1302_day_check(const uint8_t year, const uint8_t month, const uint8_t day);


static xdata char * week_list[] = {"SUN ", "MON ", "TUES", "WED ", "THUR", "FRI ", "SAT "};
static xdata ds1302_clockdata rtc_clock_data;
static xdata ds1302_day_limit[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static xdata ds1302_clockdata_limit[7][2] = {{0, 99}, {1, 12}, {1, 31}, {0, 23}, {0, 59}, {0, 59}, {1, 7}};      // clockdata 类型初始化 数值  年 月 日 时 分 秒


/**
 * @brief DS1302 初始化函数
 * 
 */
void ds1302_init(void) {
    P3   &= 0x7F;
    P3M0 &= 0x7F;
    P3M1 &= 0x7F;
}



/**
 * @brief DS1302 写时钟函数
 * 
 * @param write_data 待写入 DS1302 的日期及时钟数据( ds1302_clockdata 结构体指针 )
 */
void ds1302_write_rtc(const ds1302_clockdata *write_data) {
    data uint8_t i;
    data uint8_t write_tmp;      
    
    for (i = 0; i <= 5; i++) {
        if (ds1302_clockdata_check(write_data, i)) return;     // 待写入的日期及时钟数据 合法性检查
    }

    ds1302_spi_write(DS1302_WR_ADDR_WP, 0x00);          // 关闭写保护

    write_tmp = ((write_data->second / 10 ) << 4) | (write_data->second % 10);      // 写入 秒 数据
    ds1302_spi_write(DS1302_WR_ADDR_SEC, write_tmp);
    write_tmp = ((write_data->minute / 10 ) << 4) | (write_data->minute % 10);      // 写入 分 数据
    ds1302_spi_write(DS1302_WR_ADDR_MIN, write_tmp);
    write_tmp = ((write_data->hour / 10 ) << 4) | (write_data->hour % 10);          // 写入 时 数据
    ds1302_spi_write(DS1302_WR_ADDR_HOUR, write_tmp);
    write_tmp = ((write_data->day / 10 ) << 4) | (write_data->day % 10);            // 写入 天 数据
    ds1302_spi_write(DS1302_WR_ADDR_DAY, write_tmp);
    write_tmp = ((write_data->month / 10 ) << 4) | (write_data->month % 10);        // 写入 月 数据
    ds1302_spi_write(DS1302_WR_ADDR_MON, write_tmp);
    write_tmp = write_data->week;                                                   // 写入 星期 数据
    ds1302_spi_write(DS1302_WR_ADDR_WEEK, write_tmp);
    write_tmp = ((write_data->year / 10 ) << 4) | (write_data->year % 10);          // 写入 年 数据
    ds1302_spi_write(DS1302_WR_ADDR_YEAR, write_tmp);

    ds1302_spi_write(DS1302_WR_ADDR_WP, 0x80);          //打开写保护
}


/**
 * @brief DS1302 读时钟函数 : 读出的时钟数据写入到私有全局变量 rtc_clock_data
 * 
 */
void ds1302_read_rtc(void) {
    idata uint8_t read_tmp;

    ds1302_spi_read(DS1302_RD_ADDR_SEC, &read_tmp);
    rtc_clock_data.second = ((read_tmp >> 4) & 0x07) * 10 + (read_tmp & 0x0F);      // 读出 秒 数据
    ds1302_spi_read(DS1302_RD_ADDR_MIN, &read_tmp);
    rtc_clock_data.minute = ((read_tmp >> 4) & 0x07) * 10 + (read_tmp & 0x0F);      // 读出 分 数据
    ds1302_spi_read(DS1302_RD_ADDR_HOUR, &read_tmp);
    rtc_clock_data.hour = ((read_tmp >> 4) & 0x03) * 10 + (read_tmp & 0x0F);        // 读出 时 数据
    ds1302_spi_read(DS1302_RD_ADDR_DAY, &read_tmp);
    rtc_clock_data.day = ((read_tmp >> 4) & 0x03) * 10 + (read_tmp & 0x0F);         // 读出 天 数据
    ds1302_spi_read(DS1302_RD_ADDR_MON, &read_tmp);
    rtc_clock_data.month = ((read_tmp >> 4) & 0x01) * 10 + (read_tmp & 0x0F);       // 读出 月 数据
    ds1302_spi_read(DS1302_RD_ADDR_WEEK, &read_tmp);
    rtc_clock_data.week = read_tmp & 0x07;                                          // 读出 星期 数据
    ds1302_spi_read(DS1302_RD_ADDR_YEAR, &read_tmp);
    rtc_clock_data.year = (read_tmp >> 4 & 0x0F) * 10 + (read_tmp & 0x0F);          // 读出 年 数据

}

/**
 * @brief DS1302 实时时钟显示函数：通过 LCD1602 显示实时时钟 
 * 
 */
void ds1302_show_rtc(void) {
    //ds1302_read_rtc();
    ds1302_show_clockdata(&rtc_clock_data);
}


/**
 * @brief 获取 DS1302 实时时钟数据函数
 * 
 * @param clock_data_buf ds1302_clockdata 结构体指针
 */
void get_ds1302_rtc_data(ds1302_clockdata *clock_data_buf) {
    data uint8_t i;
    //ds1302_read_rtc();
    
    for(i = 0; i <= 6; i++){
        ((uint8_t *)clock_data_buf)[i] = ((uint8_t *)&rtc_clock_data)[i];
    }
}


/**
 * @brief 时钟数据显示函数 ： 将 ds1302_clockdata 结构体中的时钟数据通过 LCD1602 显示
 * 
 * @param clock_data ds1302_clockdata 结构体指针
 */
void ds1302_show_clockdata(const ds1302_clockdata * clock_data) {

    lcd1602_show_num_override(1, 1, clock_data->year + 2000U);      // 年
    lcd1602_show_str(1, 5, "-");
    lcd1602_show_num_override(1, 6, clock_data->month);             // 月
    lcd1602_show_str(1, 8, "-");
    lcd1602_show_num_override(1, 9, clock_data->day);               // 日
    lcd1602_show_str(1, 13, week_list[clock_data->week - 1]);       // 星期
    lcd1602_show_num_override(2, 1, clock_data->hour);              // 时
    lcd1602_show_str(2, 3, ":");
    lcd1602_show_num_override(2, 4, clock_data->minute);            // 分
    lcd1602_show_str(2, 6, ":");
    lcd1602_show_num_override(2, 7, clock_data->second);            // 秒

}


/**
 * @brief lcd1602_show_num 重载函数 : 当显示的数值小于 2 位时，用前导 0 填充至 2 位
 * 
 * @param line 显示的行号
 * @param column 显示的列号
 * @param num 待显示的无符号16位整型数据
 */
static void lcd1602_show_num_override(uint8_t line, uint8_t column, const uint16_t num) {
    if(num < 10) {lcd1602_show_char(line, column, '0'); column++;}
    lcd1602_show_num(line, column, num);
}


/**
 * @brief ds1302_clockdata 数据合法性检查
 * 
 * @param write_data ds1302_clockdata 结构体指针
 * @param check_index 检查的对象序号
 * @return uint8_t 0 ：日期及时钟数据合法     1 ：日期及时钟数据非法
 */
uint8_t ds1302_clockdata_check(const ds1302_clockdata *clock_data, const uint8_t index) {
    data uint8_t * p_clock_data = (uint8_t *)clock_data;
    data uint8_t check_value;

    if (index <= 5) {
        if (index != INDEX_DAY) {
            check_value = p_clock_data[index] < ds1302_clockdata_limit[index][LIMIT_MIN] || p_clock_data[index] > ds1302_clockdata_limit[index][LIMIT_MAX];
        }
        else {
            check_value = ds1302_day_check(p_clock_data[INDEX_YEAR], p_clock_data[INDEX_MON], p_clock_data[INDEX_DAY]);
        }
    } 

    p_clock_data[INDEX_WEEK] = ds1302_week_compute(p_clock_data[INDEX_YEAR], p_clock_data[INDEX_MON], p_clock_data[INDEX_DAY]) + 1;  // 计算 星期

    return check_value;
}


/**
 * @brief DS1302 星期计算函数 ：通过年、月、日数据计算星期数
 * 
 * @param year 年
 * @param month 月
 * @param day 日
 * @return uint8_t  星期 0 ~ 6 --> 周日 ~ 周一 ~ ... ~ 周六 
 */
uint8_t ds1302_week_compute(int16_t year, int16_t month, int16_t day) {
    data int8_t  week_tmp;
    data uint8_t century = 20;

    if (month <= 2) {
        if (--year < 0) {
            year = 99;
            century--;
        }      
    month += 12;
    }

    week_tmp = (year + (year / 4) + (century / 4) - (2 * century) +  (26 * (month + 1) / 10) + (day - 1)) % 7;      // 泰勒公式计算星期数（0~6）第1各星期时星期日
    
    return (week_tmp >= 0) ? (week_tmp) : (week_tmp + 7);       // 计算的星期数可能是复数，需要 +7 转换为正数，
}


/**
 * @brief DS1302 day数据合法性检查函数
 * 
 * @param year 年
 * @param month 月
 * @param day 日
 * @return uint8_t  0 : day 数据合法     1 : day 数据不合法
 */
static uint8_t ds1302_day_check(const uint8_t year, const uint8_t month, const uint8_t day) {
    data uint16_t year_tmp = year + 2000U;
    if (day == 0) return 1;

    if (month == 2) {
        if (year_tmp % 4 == 0 && year_tmp % 100 != 0) {       // 判断是否是闰月，如果是闰月 day应不大于29， 如果是平月 day应不大于28
            ds1302_day_limit[month - 1] = 29;
        }
        else {
            ds1302_day_limit[month - 1] = 28;
        }
    }

    return day > ds1302_day_limit[month - 1];
}


/**
 * @brief DS1302 SPI写函数 ： 将数据写入DS1302
 * 
 * @param write_addr 写操作的目标地址
 * @param write_data 待写入的数据   uint8_t类型
 */
static void ds1302_spi_write(const uint8_t write_addr, const uint8_t write_data) {
    data uint8_t i;
    DS1302_CE = 1;

    for (i = 0; i <= 7; i++) {
        DS1302_IO = (write_addr >> i)  & 0x01;
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 1;
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 0;
    }
    for (i = 0; i <= 7; i++) {
        DS1302_IO = (write_data >> i)  & 0x01;
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 1;
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 0;
    }

    DS1302_IO = 0;
    DS1302_CE = 0;
}


/**
 * @brief   DS1302 SPI读函数 ： 从DS1302读取数据
 * 
 * @param read_addr 读操作的目标地址
 * @param read_data 存储读出数据的内存指针 uint8_t *类型
 */
static void ds1302_spi_read(const uint8_t read_addr, uint8_t *const read_data) {
    data uint8_t i;
    DS1302_CE = 1;
    *read_data = 0x00;

    for (i = 0; i <= 7; i++) {
        DS1302_IO = (read_addr >> i)  & 0x01;
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 1;
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 0;
    }

    for (i = 0; i <= 7; i++) {
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 1;
        if (DS1302_IO) {*read_data |= 0x01 << i;}
        delay_1us(DS1302_SPI_DELAY);
        DS1302_SCLK = 0;
    }

    DS1302_IO = 0;
    DS1302_CE = 0;
}


/**
 * @brief 实时时钟 动态数码管显示函数
 * 
 */
void ds1302_show_rtc_7seg(void) {

    static xdata uint8_t show_index = 0;    
    switch(show_index) {
        case 0:
            led_7seg_show((rtc_clock_data.hour / 10), 1); break;            // 时 十位
        case 1:
            led_7seg_show((rtc_clock_data.hour % 10), 2); break;            // 时 个位
        case 2:
            led_7seg_show((rtc_clock_data.minute / 10), 4); break;          // 分 十位
        case 3:
            led_7seg_show((rtc_clock_data.minute % 10), 5); break;          // 分 个位
        case 4:
            led_7seg_show((rtc_clock_data.second / 10), 7); break;          // 秒 十位
        case 5:
            led_7seg_show((rtc_clock_data.second % 10), 8); break;          // 秒 个位
        default:
            break;
    }
    show_index = ++show_index % 6;
}


/**
 * @brief  时钟数据 更新函数
 * 
 * @param clock_set_data 待更新的 ds1302_clockdata 结构体指针
 * @param index 更新的对象序号  [0]年 [1]月 [2]日 [3]时 [4]分 [5]秒
 * @param update_mode 更新模式  0：VALUE_ADD  1：VALUE_DEC
 */
void clock_data_update(ds1302_clockdata *clock_data, const uint8_t index, const uint8_t update_mode) {
    data uint8_t *p_clock_data = (uint8_t *)clock_data;

    p_clock_data[index] = update_mode ? p_clock_data[index] - 1 : p_clock_data[index] + 1;
    
    if(ds1302_clockdata_check(clock_data, index)) {                     // 检查更新的 clock数据是否合法
        ds1302_clockdata_init(clock_data, index, update_mode);
    }
    
    if (index <= 1) {
        if (ds1302_clockdata_check(clock_data, INDEX_DAY)) {            // 如果更新了 年/月，再次检查天数是否合法
            ds1302_clockdata_init(clock_data, INDEX_DAY, INIT_MAX);            
        }
}
    }


/**
 * @brief ds1302_clockdata 结构体数据初始化函数
 * 
 * @param clock_data ds1302_clockdata 结构体指针
 * @param index 待始化的对象序号 [0]年 [1]月 [2]日 [3]时 [4]分 [5]秒
 * @param mode 0：表示初始化为最小值 1：表示初始化为最大值
 */
void ds1302_clockdata_init(ds1302_clockdata *clock_data, uint8_t index, uint8_t init_mode) {
    data uint8_t *p_clock_data = (uint8_t *)clock_data;
    init_mode = !!init_mode;

    if (index <= 5) {
        if (index != 2) {                        
            p_clock_data[index] = ds1302_clockdata_limit[index][init_mode];          // 初始化 年/月/时/分/秒
        }
        else {                                                                       // 初始化 日
            if (init_mode) {
                p_clock_data[index] = ds1302_day_limit[p_clock_data[INDEX_MON] - 1];
            }
            else {
                p_clock_data[index] = 1;
            }   
        }
    }

    if (index <= 2) {
        p_clock_data[INDEX_WEEK] = ds1302_week_compute(p_clock_data[INDEX_YEAR], p_clock_data[INDEX_MON], p_clock_data[INDEX_DAY]) + 1;     // 重新计算 星期数
    }
}
