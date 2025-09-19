#ifndef __MYFREERTOS_H__
#define __MYFREERTOS_H__
#include <stm32f4xx.h>
#include "tim.h"
#include "M74HC595.h"
#include "sht20.h"
#include "wifi.h"
#include "led.h"
#include "key.h"
#include "MAX30102.h"
#include "ASRPRO.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"


void freeRTOS_Cmd(void);

void start_task(void * pvParameters);

void get_sht20_task(void * pvParameters);

void digtal_display_task(void * pvParameters);

void ESP_12F_task(void * pvParameters);

void ASRPRO_task(void * pvParameters);

void get_Max30102_task(void * pvParameters);

void key_detect_task(void * pvParameters);


#endif

