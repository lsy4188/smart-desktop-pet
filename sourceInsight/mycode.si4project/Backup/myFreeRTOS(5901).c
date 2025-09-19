#include "myFreeRTOS.h"


#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;

#define TASK1_KSTACK_DEPTH 128
#define TASK1_PRIORITY 4
TaskHandle_t Task1;

#define TASK2_STACK_DEPTH 128
#define TASK2_PRIORITY 4
TaskHandle_t Task2;

#define TASK3_STACK_DEPTH 128
#define TASK3_PRIORITY 2
TaskHandle_t Task3;

#define TASK4_STACK_DEPTH 128
#define TASK4_PRIORITY 2
TaskHandle_t Task4;

void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	init_led();
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}

void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	xTaskCreate(task1, "task1", TASK1_KSTACK_DEPTH,NULL, TASK1_PRIORITY, &Task1);
	xTaskCreate(task2, "task2", TASK2_STACK_DEPTH,NULL, TASK2_PRIORITY, &Task2);
	xTaskCreate(task3, "task3", TASK3_STACK_DEPTH,NULL, TASK3_PRIORITY, &Task3);
	xTaskCreate(task4, "task4", TASK4_STACK_DEPTH,NULL, TASK4_PRIORITY, &Task4);
	vTaskDelete(NULL);//删除自身
	taskEXIT_CRITICAL();//离开临界区
}

void task1(void * pvParameters){
	while(1){
		printf("task1 running.........\r\n");
		GPIO_ToggleBits(GPIOC,LED1);
		vTaskSuspend(Task1);
		vTaskDelay(500);
	}
}

void task2(void * pvParameters){
	while(1){
		printf("task2 running.........\r\n");
		GPIO_ToggleBits(GPIOC,LED2);
		vTaskDelay(500);
	}
}

void task3(void * pvParameters){
	while(1){
		printf("task3 running.........\r\n");
		GPIO_ToggleBits(GPIOC,LED3);
		vTaskSuspend(Task2);
		vTaskDelay(500);
	}
}
void task4(void * pvParameters){
	while(1){
		printf("task4 running.........\r\n");
		GPIO_ToggleBits(GPIOF,GPIO_Pin_8);
		vTaskResume(Task1);
		vTaskResume(Task2);
		vTaskDelay(500);
	}
}



