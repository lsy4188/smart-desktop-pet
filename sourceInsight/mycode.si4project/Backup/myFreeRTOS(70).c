#include "myFreeRTOS.h"

#define STACK_DEPTH 128
#define PRIORITY 1
TaskHandle_t masterTask;
/*传感器任务*/
#define SHT20_STACK_DEPTH 384
#define SHT20_PRIORITY 2
TaskHandle_t SHT20_TASK;


#define MAX30102_STACK_DEPTH 384
#define MAX30102_PRIORITY 2
TaskHandle_t MAX30102_TASK;

/*显示任务*/
#define DIGTAL_STACK_DEPTH 130
#define DIGTAL_PRIORITY 4
TaskHandle_t DIGTAL_DISPLAY_TASK;

#define LCD_STACK_DEPTH 1024
#define LCD_PRIORITY 4
TaskHandle_t LCD_DISPLAY_TASK;

/*控制任务*/
#define KEY_STACK_DEPTH 256
#define KEY_PRIORITY 4
TaskHandle_t KEY_DETECT_TASK;

#define ASRPRO_STACK_DEPTH 256
#define ASRPRO_PRIORITY 3
TaskHandle_t ASRPRO_CTRL_TASK;

#define WIFI_STACK_DEPTH 256
#define WIFI_PRIORITY 3
TaskHandle_t WIFI_CTRL_TASK;


void freeRTOS_Cmd(void){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	init_tim6_basic();//基本延迟
	init_tim7_basic();//ESP-12F
	init_tim2_common();//sht20中断
	init_tim4_common();//刷新数码管
	
	init_usart6();//串口打印
	init_usart2();//ASRPRO
	init_usart3();//ESP-12F
	init_i2c();//sht20,Max30102
	init_spi();//HS280S010B,数码管
	
	xTaskCreate(start_task, "start_task", STACK_DEPTH,NULL, PRIORITY, &masterTask);
	vTaskStartScheduler();
}

void start_task(void * pvParameters){
	taskENTER_CRITICAL();//进入临界区(临界区中断不会被打断)	
	//xTaskCreate(digtal_display_task, "digtal_display_task", DIGTAL_STACK_DEPTH,NULL, DIGTAL_PRIORITY, &DIGTAL_DISPLAY_TASK);
	//xTaskCreate(lcd_display_task, "lcd_display_task", LCD_STACK_DEPTH, NULL, LCD_PRIORITY, &LCD_DISPLAY_TASK);
	//xTaskCreate(ASRPRO_task, "ASRPRO_task", ASRPRO_STACK_DEPTH,NULL, ASRPRO_PRIORITY, &ASRPRO_CTRL_TASK);
	//xTaskCreate(ESP_12F_task, "ESP_12F_task", WIFI_STACK_DEPTH,NULL, WIFI_PRIORITY, &WIFI_CTRL_TASK);
	xTaskCreate(key_detect_task, "key_detect_task", KEY_STACK_DEPTH,NULL, KEY_PRIORITY, &KEY_DETECT_TASK);
	//xTaskCreate(get_Max30102_task, "get_Max30102_task", MAX30102_STACK_DEPTH,NULL, MAX30102_PRIORITY, &MAX30102_TASK);
	//xTaskCreate(get_sht20_task, "get_sht20_task", SHT20_STACK_DEPTH,NULL, SHT20_PRIORITY, &SHT20_TASK);
	
	taskEXIT_CRITICAL();//离开临界区
	vTaskDelete(NULL);//删除自身
}

void key_detect_task(void * pvParameters){
	uint32_t key;
	init_key_INT();
	while(1){
		printf("------->key_detect_task\r\n");
		if(xTaskNotifyWait(0, 0xffffffff, &key, portMAX_DELAY)){
			if (key == KEY1) {
                GPIO_ToggleBits(GPIOC, LED1);
				//xTaskNotify(WIFI_CTRL_TASK, KEY1, eSetValueWithOverwrite);
            } else if (key == KEY3) {
                GPIO_ToggleBits(GPIOC, LED3);
				//xTaskNotify(WIFI_CTRL_TASK, KEY3, eSetValueWithOverwrite);
            }
		}
	}
}

/**************************控制任务*******************************/
void ESP_12F_task(void * pvParameters){
	uint32_t data;
	init_wifi_mode();
	printf("------->ESP_12F_task\r\n");
	while(1){
		if(xTaskNotifyWait(0, 0xffffffff, &data, 0)==pdTRUE){
			if(data==KEY1){
				set_wifi_mode(CONNECT);
			}else{
				set_wifi_mode(DISCONNECT);
			}
		}
		update_wifi_mode();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void ASRPRO_task(void * pvParameters){
	uint32_t data;
	init_ASRPRO();
	while(1){
		if(xTaskNotifyWait(0, 0xffffffff, &data, portMAX_DELAY)){
			deal_with_asr();
		}
	}
}

/**********************数据显示任务**************************/
void digtal_display_task(void * pvParameters){
	init_M74HC595();
	printf("------->digtal_display_task\r\n");
	while (1){
		digital_tube_refresh();
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

void lcd_display_task(void * pvParameters){
	init_lcd(LCD_LANDSCAPE);
	lcd_clear();
	printf("------->lcd_display_task\r\n");
	while(1){	
		disp_by_LCD();
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}


/********************************传感器任务************************/
void get_sht20_task(void * pvParameters){
	uint32_t data;	
	init_SHT20V1();
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	printf("------->get_sht20_task\r\n");
	while (1){
		xTaskNotifyWait(0, 0xffffffff, &data, portMAX_DELAY);
		taskENTER_CRITICAL();
		if(data==1){
			Process_DataV1("hold");
		}
		taskEXIT_CRITICAL();
	}
}


void get_Max30102_task(void * pvParameters){
	uint32_t data;
	UBaseType_t task_priority;
	printf("------->get_Max30102_task\r\n");
	while(1){
		xTaskNotifyWait(0, 0xffffffff, &data, portMAX_DELAY);
		if(data&BIT_0){
			task_priority=uxTaskPriorityGet(NULL);//获取当前任务优先级
			vTaskPrioritySet(NULL, 5);//重新设置当前任务优先级
			MAX30102_cmd();
			while(!get_SpO2_HR());
			Stop_MAX30102_Sampling();
			vTaskPrioritySet(NULL, task_priority);//重新设置当前任务优先级
		}
	}
}




