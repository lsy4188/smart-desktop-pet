#include "usart.h"
#include "wifi.h"
#include "sht20.h"
#include "tim.h"
#include "ASRPRO.h"
#include "M74HC595.h"
#include "sg90.h"
#include "max30102.h"
#include "myFreeRTOS.h"
#include "lcd.h"

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

#ifdef 0

extern wifi_buffer wb;
int main(void)
{	
	init_wifi_mode();
	while(1){
		wifi_connect_process();
		
	}
}



int main(void)
{
	init_ASRPRO();
	while(1){
		deal_with_asr();
		update_display();
		update_action();
		update_wifi_mode();
		update_SpO2();
	}
}

int main(void)
{
	init_tim6_basic();
	init_sg90(1);
	while(1){
	}
}

int main(void)
{
	init_tim6_basic();
	init_usart6();
	init_MAX30102();
	delay_ms(1000);
	Start_MAX30102_Sampling();
	printf("Sampling started.!!!");
	while(1){
		get_real_SpO2();
    }
}




extern sht20 sh;
int main(void)
{
	init_M74HC595();
	init_SHT20();
	while(1){
		if(sh.flg){
			Process_Data("hold");
			display_sensor(sh.tem);
		}
		display_update();
	}
}

int main(void){
	freeRTOS_Cmd();
}



#endif

extern sht20 sh;
int main(void)
{
	char temp[10];
	init_tim6_basic();
	init_tim2_common();
	init_usart6();
	init_SHT20();
	init_lcd();
	lcd_clear();
	while(1){
		if(sh.flg){
			memset(temp,0,sizeof(temp));
			Process_Data("hold");
			sprintf(temp,"%.2f",sh.tem);
			lcd_write_str(10, 10, temp, 0x07E0, 0xFFFF, 2);
			memset(temp,0,sizeof(temp));
			sprintf(temp,"%.2f",sh.rh);
			lcd_write_str(150, 10, temp, 0x07E0, 0xFFFF, 2);
		}
		
	}
}









