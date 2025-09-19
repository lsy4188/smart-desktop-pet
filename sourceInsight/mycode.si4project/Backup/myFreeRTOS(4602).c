#include "myFreeRTOS.h"

#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;

#define SHT20_STACK_DEPTH 128
#define SHT20_PRIORITY 2
TaskHandle_t SHT20_TEM_TASK;
TaskHandle_t SHT20_RH_TASK;

#define DIGTAL_STACK_DEPTH 128
#define DIGTAL_PRIORITY 4
TaskHandle_t DIGTAL_DISPLAY_TASK;

#define WIFI_STACK_DEPTH 128
#define WIFI_PRIORITY 4
TaskHandle_t WIFI_CTRL_TASK;

#define BIT_0 (1<<0);
#define BIT_1 (1<<1);

void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	init_led();
	init_wifi_mode();
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}


void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	xTaskCreate(get_rh_task, "get_rh_task", SHT20_STACK_DEPTH,NULL, SHT20_PRIORITY, &SHT20_RH_TASK);
	xTaskCreate(get_tem_task, "get_tem_task", SHT20_STACK_DEPTH,NULL, SHT20_PRIORITY, &SHT20_TEM_TASK);
	xTaskCreate(digtal_display_task, "digtal_display_task", DIGTAL_STACK_DEPTH,NULL, DIGTAL_PRIORITY, &DIGTAL_DISPLAY_TASK);
	//xTaskCreate(ESP_12F_task, "ESP_12F_task", WIFI_STACK_DEPTH,NULL, WIFI_PRIORITY, &WIFI_CTRL_TASK);
	vTaskDelete(NULL);//删除自身
	taskEXIT_CRITICAL();//离开临界区
}


void digtal_display_task(void * pvParameters){
	uint32_t flg;
	while (1)
		{
		GPIO_ToggleBits(GPIOC,LED3);
		//xTaskNotifyWait(0, 0x00, &flg, portMAX_DELAY);
		digital_tube_refresh();
		vTaskDelay(1);
	}
}

extern sht20 sh;
void get_tem_task(void * pvParameters){
	BaseType_t res;
	while (1)
		{
		GPIO_ToggleBits(GPIOC,LED2);
		if(sh.flg){
			Process_Data("hold");
			display_sensor(sh.tem);
			//res=xTaskNotify(DIGTAL_DISPLAY_TASK, BIT_0, eSetBits);
			if(res==pdFALSE){
				printf("send BIT_0 error\r\n");
			}
		}
	vTaskDelay(1);
	}
}

void get_rh_task(void * pvParameters){
	while (1)
		{
		GPIO_ToggleBits(GPIOC, LED1);
		if(sh.flg){
			Process_Data("hold");
			display_sensor(sh.rh);
			//res=xTaskNotify(DIGTAL_DISPLAY_TASK, BIT_1, eSetBits);
			/*if(res==pdFALSE){
				printf("send BIT_1 error\r\n");
			}*/
		}	
	vTaskDelay(1);
	}

}

void ESP_12F_task(void * pvParameters){
	while(1){
		taskENTER_CRITICAL();
		wifi_connect_process();
		taskEXIT_CRITICAL();
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



