#include "myFreeRTOS.h"

#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;
/*传感器任务*/
#define SHT20_STACK_DEPTH 128
#define SHT20_PRIORITY 2
TaskHandle_t SHT20_TASK;

#define MAX30102_STACK_DEPTH 128
#define MAX30102_PRIORITY 2
TaskHandle_t MAX30102_TASK;

/*显示任务*/
#define DIGTAL_STACK_DEPTH 128
#define DIGTAL_PRIORITY 4
TaskHandle_t DIGTAL_DISPLAY_TASK;

/*控制任务*/
#define KEY_STACK_DEPTH 128
#define KEY_PRIORITY 3
TaskHandle_t KEY_DETECT_TASK;

#define ASRPRO_STACK_DEPTH 128
#define ASRPRO_PRIORITY 3
TaskHandle_t ASRPRO_CTRL_TASK;

#define WIFI_STACK_DEPTH 128
#define WIFI_PRIORITY 4
TaskHandle_t WIFI_CTRL_TASK;




void freeRTOS_Cmd(void){
	init_M74HC595();
	init_SHT20();
	init_led();
	init_key_INT();
	init_wifi_mode();
	init_MAX30102();
	init_ASRPRO();
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}

void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)
	xTaskCreate(get_Max30102_task, "get_Max30102_task", MAX30102_STACK_DEPTH,NULL, MAX30102_PRIORITY, &MAX30102_TASK);
	xTaskCreate(get_sht20_task, "get_sht20_task", SHT20_STACK_DEPTH,NULL, SHT20_PRIORITY, &SHT20_TASK);
	xTaskCreate(digtal_display_task, "digtal_display_task", DIGTAL_STACK_DEPTH,NULL, DIGTAL_PRIORITY, &DIGTAL_DISPLAY_TASK);
	xTaskCreate(ESP_12F_task, "ESP_12F_task", WIFI_STACK_DEPTH,NULL, WIFI_PRIORITY, &WIFI_CTRL_TASK);
	xTaskCreate(ASRPRO_task, "ASRPRO_task", WIFI_STACK_DEPTH,NULL, WIFI_PRIORITY, &ASRPRO_CTRL_TASK);
	xTaskCreate(key_detect_task, "key_detect_task", KEY_STACK_DEPTH,NULL, KEY_PRIORITY, &KEY_DETECT_TASK);
	vTaskDelete(NULL);//删除自身
	taskEXIT_CRITICAL();//离开临界区
}

void ESP_12F_task(void * pvParameters){
	uint32_t value;
	BaseType_t res;
	while(1){
		/*taskENTER_CRITICAL();
		wifi_connect_process();
		taskEXIT_CRITICAL();*/
		printf("=======>>>>esp-12f_task<<<<=====\r\n");
		vTaskDelay(100);
	}
}

void ASRPRO_task(void * pvParameters){
	while(1){
		printf("=======>>>>asrpro_task<<<<=====\r\n");
			vTaskDelay(100);
	}
}

void key_detect_task(void * pvParameters){
	uint8_t key;
	while(1){
		key=detect_key();
		if(key==KEY1){
			GPIO_ToggleBits(GPIOC,LED1);
		}else if(key==KEY3){
			GPIO_ToggleBits(GPIOC,LED3);
		}
			vTaskDelay(50);
	}

}

void digtal_display_task(void * pvParameters){
	while (1)
		{
		/*GPIO_ToggleBits(GPIOC,LED3);
		digital_tube_refresh();
		vTaskDelay(1);*/
		printf("=======>>>>display_task<<<<=====\r\n");
		vTaskDelay(100);
	}
}

extern sht20 sh;
void get_sht20_task(void * pvParameters){
	BaseType_t res;
	while (1){
		/*GPIO_ToggleBits(GPIOC,LED2);
		if(sh.flg){
			Process_Data("hold");
			//display_sensor(sh.tem);
		}
	vTaskDelay(1);*/
	printf("=======>>>>sht20<<<<=====\r\n");
	vTaskDelay(100);
	}
}

void get_Max30102_task(void * pvParameters){
	while(1){
		printf("=======>>>>Max30102<<<<=====\r\n");
			vTaskDelay(100);
	}
}





