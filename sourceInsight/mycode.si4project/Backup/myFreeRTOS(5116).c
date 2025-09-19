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
#define TASK3_PRIORITY 4
TaskHandle_t Task3;

QueueHandle_t tem_queue;
QueueHandle_t rh_queue;
QueueSetHandle_t queue_set;

#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
EventGroupHandle_t event_group_handle;


void create_queue_set(void){
	tem_queue=xQueueCreate(2, sizeof(double));
	rh_queue=xQueueCreate(2, sizeof(double));
	queue_set=xQueueCreateSet(2);
	xQueueAddToSet(tem_queue, queue_set);
	xQueueAddToSet(rh_queue, queue_set);
}

void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	init_led();
	//create_queue_set();
	event_group_handle=xEventGroupCreate();
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
		taskENTER_CRITICAL();
		GPIO_ToggleBits(GPIOC,LED1);
		if(sh.flg){
			Process_Data("hold");
			//xQueueSend(tem_queue, &sh.tem, portMAX_DELAY);
			xEventGroupSetBits(event_group_handle, BIT_0);
		}
		taskEXIT_CRITICAL();
		vTaskDelay(600);
	}
}

void task2(void * pvParameters){
	while(1){
		taskENTER_CRITICAL();
		GPIO_ToggleBits(GPIOC,LED2);
		if(sh.flg){
			Process_Data("hold");
			//xQueueSend(rh_queue, &sh.rh, portMAX_DELAY);
			xEventGroupSetBits(event_group_handle, BIT_1);
		}
		taskEXIT_CRITICAL();
		vTaskDelay(500);
	}
}

void task3(void * pvParameters){
	double temp=0;
	QueueSetMemberHandle_t queueHandle;
	EventBits_t even_bit;
	while(1){
		GPIO_ToggleBits(GPIOC,LED3);
		/*queueHandle=xQueueSelectFromSet(queue_set, portMAX_DELAY);
		xQueueReceive(queueHandle, &temp, portMAX_DELAY);
		if(queueHandle==tem_queue){
			printf("get tem_queue data---->%.2lf\r\n",temp);
		}else{
			printf("get rh_queue data------>%.2lf\r\n",temp);
		}*/
		even_bit=xEventGroupWaitBits(event_group_handle, 
							BIT_0|BIT_1, 
							pdTRUE, 
							pdTRUE, 
							portMAX_DELAY);
		printf("even_bit=%d\r\n",even_bit);
		vTaskDelay(100);
	}
}
//数码管显示(优先级要高)
/*void task1(void * pvParameters){
	while(1){
		GPIO_ToggleBits(GPIOC,LED1);
		digital_tube_refresh();
		vTaskDelay(1);
	}
}
extern sht20 sh;
void task2(void * pvParameters){
	while(1){
		GPIO_ToggleBits(GPIOC,LED2);
		if(sh.flg){
			Process_Data("hold");
			display_sensor(sh.tem);
		}
		vTaskDelay(1);
	}
}*/



