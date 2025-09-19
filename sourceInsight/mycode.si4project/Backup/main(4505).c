#include "usart.h"
#include "i2c.h"
#include "SHT20.h"
#include "M74HC595.h"
#include "systick.h"
#include "MAX30102.h"
#include "tim.h"
#include "dma.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
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

//主任务
uint8_t num;
#define STACK_SIZE 128
TaskHandle_t  master;
TaskHandle_t  task1;
TaskHandle_t  task2;

void task_one( void * pvParameters ){
	printf("%s\r\n",__FUNCTION__);
	for(;;){
		num++;
		vTaskDelay(100);//延迟(阻塞)
	}
}
void task_two( void * pvParameters ){
	printf("%s\r\n",__FUNCTION__);
	for(;;){
		num++;
		vTaskDelay(100);
	}
}

void task_master( void * pvParameters ){
	printf("%s\r\n",__FUNCTION__);
	taskENTER_CRITICAL();//进入临界区保护
	xTaskCreate(task_one, "task_one", STACK_SIZE, NULL, 1, &task1);
	xTaskCreate(task_two, "task_two", STACK_SIZE, NULL, 2, &task2);
	taskEXIT_CRITICAL();//退出临界区保护
	vTaskDelete(master);
}

int main(void)
{
	init_systick(168);
	init_usart();
	printf("test starting!!!");
	xTaskCreate(task_master, "task_master", STACK_SIZE, NULL, 1, &master);
	vTaskStartScheduler();
	while(1){
		printf("num=%d\r\n",num);
	}
}
