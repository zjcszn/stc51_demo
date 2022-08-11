#include "../inc/framework/task.h"
#include "../inc/mcu/mcu.h"
#include "../inc/device/lcd1602.h"

#define TASK_MAX_SIZE   6


static xdata uint8_t current_task_id;


typedef struct _task_struct {
    // 任务指针
    void (*p_task)(void);
    // 任务延时
    uint16_t task_delay;
    uint16_t task_period;
    uint8_t run_flag;
}task_struct;

static idata task_struct task_list[TASK_MAX_SIZE] = {0}; 



uint8_t get_current_task_id(void) {
    return current_task_id;
}

uint8_t task_sch_add(void (*p_func)(void), const uint16_t delay, const uint16_t preiod) {
    uint8_t index = 0;
    while ((task_list[index].p_task != 0) && (index < TASK_MAX_SIZE)) {
        index++;
    }
    if (index == TASK_MAX_SIZE) {
        lcd1602_clear();
        lcd1602_show_str(1, 1, "TASK LIST FULL");
        return 255;
    }
    task_list[index].p_task = p_func;
    task_list[index].task_delay = delay;
    task_list[index].task_period = preiod;
    task_list[index].run_flag = 0;
    return index;
}

void task_sch_dispatch(void) {
    uint8_t index;
    for (index = 0; index < TASK_MAX_SIZE; index++) {

        if (task_list[index].run_flag > 0) {

            task_list[index].run_flag = 0;
            current_task_id = index;                            // 设置当前被调度函数的index
            (*task_list[index].p_task)();                       // 函数调度

            if (task_list[index].task_period == 0) {
                task_sch_del(index);
            }

        }
    }
}

void task_sch_del(uint8_t index) {
    task_list[index].p_task = 0;
    task_list[index].task_delay = 0;
    task_list[index].task_period = 0;
    task_list[index].run_flag = 0;
}

void task_sch_update(void) {
    uint8_t index;
    for (index = 0; index < TASK_MAX_SIZE; index++) {
        if (task_list[index].p_task) {
            if (task_list[index].task_delay == 0 ) {
                task_list[index].run_flag = 1;
                if (task_list[index].task_period > 0) {
                    task_list[index].task_delay = task_list[index].task_period;
                }

            }
            else {  
                task_list[index].task_delay--;
            }
        }

    }
}

