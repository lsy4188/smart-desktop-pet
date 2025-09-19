#ifndef __MYFREERTOS_H__
#define __MYFREERTOS_H__
#include <stm32f4xx.h>
#include "tim.h"
#include "M74HC595.h"
#include "sht20.h"
#include "FreeRTOS.h"
#include "task.h"

#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;

#define TASK1_KSTACK_DEPTH 128
#define TASK1_PRIORITY 1
TaskHandle_t Task1;

#define TASK2_STACK_DEPTH 128
#define TASK2_PRIORITY 1
TaskHandle_t Task2;

void freeRTOS_Cmd(void);

void start_task(void);

void task1(void);

void task2(void);






#endif

