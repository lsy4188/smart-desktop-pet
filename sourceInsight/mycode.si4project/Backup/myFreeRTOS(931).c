#include "myFreeRTOS.h"

#define QUEUE_LENGTH 128
#define ITEM_SIZE 128
QueueHandle_t queueHandle;


#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;

#define TASK1_KSTACK_DEPTH 128
#define TASK1_PRIORITY 3
TaskHandle_t Task1;

#define TASK2_STACK_DEPTH 128
#define TASK2_PRIORITY 3
TaskHandle_t Task2;


void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	init_led();
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}


void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	queueHandle=xQueueCreate(1, sizeof(sht20));
	xTaskCreate(task1, "task1", TASK1_KSTACK_DEPTH,NULL, TASK1_PRIORITY, &Task1);
	xTaskCreate(task2, "task2", TASK2_STACK_DEPTH,NULL, TASK2_PRIORITY, &Task2);
	vTaskDelete(NULL);//删除自身
	taskEXIT_CRITICAL();//离开临界区
}

extern sht20 sh;
void task1(void * pvParameters){
	while(1){
		GPIO_ToggleBits(GPIOC,LED1);
		if(sh.flg){
			Process_Data("hold");
			xQueueSend(queueHandle, &sh, portMAX_DELAY);
		}
		vTaskDelay(500);
	}
}

void task2(void * pvParameters){
	sht20 receive;
	static uint8_t display_flg=0;
	while(1){
		GPIO_ToggleBits(GPIOC,LED2);
		xQueueReceive(queueHandle,&receive,portMAX_DELAY);
		if(display_flg){
			display_sensor(receive.tem);
		}else{
			display_sensor(receive.rh);
		}
		display_flg=~display_flg;
		vTaskDelay(500);
	}
}


