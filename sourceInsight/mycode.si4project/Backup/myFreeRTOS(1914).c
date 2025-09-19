#include "myFreeRTOS.h"

void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	xTaskCreate(
		start_task, 
		"start_task", 
		STACK_DEPTH,
		NULL, 
		PRIORITY, 
		&masterTask
		);
	vTaskStartScheduler(void);
}

void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	xTaskCreate(task1, "task1", TASK1_KSTACK_DEPTH,NULL, TASK1_PRIORITY, &Task1);
	xTaskCreate(task2, "task2", TASK2_STACK_DEPTH,NULL, TASK2_PRIORITY, &Task2);
	vTaskDelete(NULL);
	taskEXIT_CRITICAL();//离开临界区
}

void task1(void * pvParameters){
	while(1){
		printf("task1 running.........\r\n");
	}
}

void task2(void * pvParameters){
	while(1){
		printf("task2 running.........\r\n");
	}
}




