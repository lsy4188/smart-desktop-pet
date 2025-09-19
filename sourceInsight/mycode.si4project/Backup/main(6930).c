#include "systick.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "fan.h"
#include "usart.h"
#include "tim.h"
#include "i2c.h"
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

int main(void)
{
	init_led();
	init_key_INT();
	init_beep();
	init_fan();
	init_usart();
	init_tim();
	init_i2c();
	printf("Hello World!!!!!\n");
	while(1){ 
		if(cmd_ready){
			parse_command();
		}
		switch (mode)
			{
			case 1:
				if(key_flag&(1<<LED_BIT)){
					turn_on_ledx(LED1);
				}else{
					turn_off_ledx(LED1);
				}
			
				break;
			case 2:
				if(key_flag&(1<<BEEP_BIT)){
					beep_open();
				}else{
					beep_close();
				}
			
				break;
			case 3:
				if(key_flag&(1<<FAN_BIT)){
					fan_on();
				}else{
					fan_off();
				}
			
				break;
			}
	}
}

