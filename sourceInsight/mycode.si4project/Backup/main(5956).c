#include "usart.h"
#include "i2c.h"
#include "led.h"
#include "SHT20.h"
#include "M74HC595.h"
#include "systick.h"
#include "MAX30102.h"
#include "tim.h"
#include "dma.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#define LED_BIT  0
#define BEEP_BIT 1
#define FAN_BIT  2
u8 key_flag=0;
u8 mode=0;
/*
定义通讯规则
*/
#define CMD_BUFFER_SIZE 32
u8 cache[CMD_BUFFER_SIZE];
u16 cmd_ready=0;
u16 cmd_index=0;
//sht20数据就绪标记位
u8 sht20_flg=0;
sht20 st;

//主任务
#define STACK_SIZE 128
#define START_TASK_PRIORITY 0
TaskHandle_t  master;
//任务一
#define task1_STACK_SIZE 128
#define task1_TASK_PRIORITY 1
TaskHandle_t  task1;
//任务二
#define task2_STACK_SIZE 128
#define task2_TASK_PRIORITY 0
TaskHandle_t  task2;

//队列元素
#define QUEUE_SIZE 10
QueueHandle_t queue_handle;


void task_one( void * pvParameters ){
	for(;;){
		printf("%s\r\n",__FUNCTION__);
		st=Process_Data("hold");
		xQueueSend(queue_handle, &st, portMAX_DELAY);
		}	
		vTaskDelay(100);//延迟(阻塞)
	}

void task_two( void * pvParameters ){
	sht20 value;
	for(;;){
		printf("%s\r\n",__FUNCTION__);
		xQueueReceive(queue_handle, &value, portMAX_DELAY);
	//	display_sensor(value.tem);
		vTaskDelay(100);//延迟(阻塞)
	}
}

void task_master( void * pvParameters ){
	printf("%s\r\n",__FUNCTION__);
	taskENTER_CRITICAL();//进入临界区保护
	queue_handle=xQueueCreate(QUEUE_SIZE, sizeof(sht20));
	xTaskCreate(task_one, "task_one", task1_STACK_SIZE, NULL, task1_TASK_PRIORITY, &task1);
	xTaskCreate(task_two, "task_two", task2_STACK_SIZE, NULL, task2_TASK_PRIORITY, &task2);
	vTaskDelete(master);
	taskEXIT_CRITICAL();//退出临界区保护
}

int main(void)
{	
	init_systick(168);
	init_usart();
	init_tim_basic();
	init_SHT20();
	init_M74HC595();
	xTaskCreate(task_master, "task_master", STACK_SIZE, NULL, START_TASK_PRIORITY, &master);
	vTaskStartScheduler();
	while(1){
	}
}

