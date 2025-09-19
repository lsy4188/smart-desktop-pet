#include "myFreeRTOS.h"

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
	queueHandle=xQueueCreate(1, sizeof(char*));
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}


void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	xTaskCreate(task1, "task1", TASK1_KSTACK_DEPTH,NULL, TASK1_PRIORITY, &Task1);
	xTaskCreate(task2, "task2", TASK2_STACK_DEPTH,NULL, TASK2_PRIORITY, &Task2);
	vTaskDelete(NULL);//删除自身
	taskEXIT_CRITICAL();//离开临界区
}

extern sht20 sh;
void task1(void * pvParameters){
	char *p="sadegfesrgrehtyjh123456789jhluig";
	while(1){
		GPIO_ToggleBits(GPIOC,LED1);
		xQueueSend(queueHandle, &p, portMAX_DELAY);
		vTaskDelay(500);
	}
}

void task2(void * pvParameters){
	char*q=NULL;
	while(1){
		GPIO_ToggleBits(GPIOC,LED2);
		if(xQueueReceive(queueHandle,&q,portMAX_DELAY)){
			printf("接收到的字符串为:%s\r\n",*q);
		}
		vTaskDelay(500);
	}
}


