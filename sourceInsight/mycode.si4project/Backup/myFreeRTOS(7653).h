#ifndef __MYFREERTOS_H__
#define __MYFREERTOS_H__
#include <stm32f4xx.h>
#include "tim.h"
#include "M74HC595.h"
#include "sht20.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"


void freeRTOS_Cmd(void);

void start_task(void * pvParameters);

void task1(void * pvParameters);

void task2(void * pvParameters);

void task3(void * pvParameters);

void create_queue_set(void);








#endif

