#include "myFreeRTOS.h"

#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;

#define TASK1_KSTACK_DEPTH 128
#define TASK1_PRIORITY 3
TaskHandle_t Task1;

#define TASK2_STACK_DEPTH 128
#define TASK2_PRIORITY 3
TaskHandle_t Task2;

#define TASK3_STACK_DEPTH 128
#define TASK3_PRIORITY 3
TaskHandle_t Task3;

QueueHandle_t tem_queue;
QueueHandle_t rh_queue;
QueueSetHandle_t queue_set;



void create_queue_set(void){
	tem_queue=xQueueCreate(1, sizeof(double));
	rh_queue=xQueueCreate(1, sizeof(double));
	queue_set=xQueueCreateSet(2);
	xQueueAddToSet(tem_queue, queue_set);
	xQueueAddToSet(rh_queue, queue_set);
}

void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	init_led();
	create_queue_set();
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}


void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	xTaskCreate(task1, "task1", TASK1_KSTACK_DEPTH,NULL, TASK1_PRIORITY, &Task1);
	xTaskCreate(task2, "task2", TASK2_STACK_DEPTH,NULL, TASK2_PRIORITY, &Task2);
	xTaskCreate(task3, "task3", TASK3_STACK_DEPTH,NULL, TASK3_PRIORITY, &Task3);
	vTaskDelete(NULL);//删除自身
	taskEXIT_CRITICAL();//离开临界区
}

extern sht20 sh;
void task1(void * pvParameters){
	while(1){
		GPIO_ToggleBits(GPIOC,LED1);
		if(sh.flg){
			Process_Data("hold");
			xQueueSend(tem_queue, &sh.tem, NULL);
		}
		vTaskDelay(100);
	}
}

void task2(void * pvParameters){
	while(1){
		GPIO_ToggleBits(GPIOC,LED2);
		if(sh.flg){
			Process_Data("hold");
			xQueueSend(rh_queue, &sh.rh, NULL);
		}
		vTaskDelay(100);
	}
}

void task3(void * pvParameters){
	double temp;
	while(1){
		GPIO_ToggleBits(GPIOC,LED3);
		QueueSetMemberHandle_t queueHandle=xQueueSelectFromSet(queue_set, NULL);
		xQueueReceive(queueHandle, &temp, NULL);
		if(queueHandle==tem_queue){
			printf("get tem_queue data---->%.2lf\r\n",temp);
		}else{
			printf("get rh_queue data------>%.2lf\r\n",temp);
		}
		vTaskDelay(500);
	}
}


