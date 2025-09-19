#ifndef __MYFREERTOS_H__
#define __MYFREERTOS_H__
#include <stm32f4xx.h>
#include "FreeRTOS.h"
#include "task.h"

#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;

void start_task(void);
void freeRTOS_Cmd(void);


#endif

