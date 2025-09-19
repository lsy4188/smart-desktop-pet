#include "systick.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "fan.h"
#define LED_BIT  0
#define BEEP_BIT 1
#define FAN_BIT  2
u8 key_flag=0;
u8 mode=0;
int main(void)
{
	init_led();
	init_key_INT();
	init_beep();
	init_fan();
	while(1){ 
		switch (mode)
			{
			case 1:
				if(key_flag_led){
					turn_on_ledx(LED1);
				}else{
					turn_off_ledx(LED1);
				}
			
				break;
			case 2:
				if(key_flag_beep){
					beep_open();
				}else{
					beep_close();
				}
			
				break;
			case 3:
				if(key_flag_fan){
					fan_on();
				}else{
					fan_off();
				}
			
				break;
			}
	}
}

