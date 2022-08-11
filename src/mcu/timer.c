#include "../inc/mcu/timer.h"
#include "../inc/mcu/mcu.h"
#include "../inc/device/ds1302.h"
#include "../inc/framework/task.h"
#include "../inc/framework/keyscan.h"


void timer_init(void) {
	timer0_init();
	timer1_init();
	uart_timer2_init();
	EA = 1;
}




//  定时器 0 初始化 @ 10ms 24Mhz
void timer0_init(void) {

	AUXR &= 0x7F;		// 定时器时钟12T模式
	TMOD &= 0xF0;		// 设置定时器模式
	TMOD |= 0x03;		// 不可屏蔽中断16位自动重载模式
	TL0 = 0xE0;			// 设置定时初始值
	TH0 = 0xB1;			// 设置定时初始值
	TF0 = 0;			// 清除TF0标志
	TR0 = 1;			// 定时器0开始计时
	ET0 = 1;
}

//	任务调度
void timer0_task_isr() interrupt 1 {
    task_sch_update();
}


// 定时器 1 初始化 @ 2毫秒 24.000MHz
void timer1_init(void) {

	AUXR |= 0x40;		// 定时器时钟1T模式
	TMOD &= 0x0F;		// 设置定时器模式
	TL1 = 0x80;			// 设置定时初始值
	TH1 = 0x44;			// 设置定时初始值
	TF1 = 0;			// 清除TF1标志
	TR1 = 1;			// 定时器1开始计时
	PT1 = 0;			// 定时器优先级设置 3级
	IPH |= 0x08;
	ET1 = 1;
}


//	键盘扫描 及 动态数码管 调度
void timer1_isr() interrupt 3 {
	static uint8_t timer1_cnt = 0;
    if (++timer1_cnt == 5) {
		timer1_cnt = 0;
		key_scan();
	}
	ds1302_show_rtc_7seg();
}


//	定时器 2 初始化	 @ 串口定时器 波特率 115200 时钟频率 24 Mhz
void uart_timer2_init(void) {

	AUXR |= 0x01;		// 串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		// 定时器时钟1T模式
	T2L = 0xCC;			// 设置定时初始值
	T2H = 0xFF;			// 设置定时初始值
	IP |= 0x10;			// UART定时器优先级设置 2级
	IPH &= 0xEF;
	AUXR |= 0x10;		// 定时器2开始计时
}

