#include "systick.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "fan.h"
u8 key_flag=0;
u8 mode=0;
int main(void)
{
	init_led();
	init_systick(168);
	init_key();
	init_beep();
	init_fan();
	while(1){ 
		get_key_val_same(3);
		switch (mode)
			{
			case 1:{
				if(key_flag){
					turn_on_ledx(LED1);
				}else{
					turn_off_ledx(LED1);
				}
			}
				break;
			case 2:{
				if(key_flag){
					beep_open();
				}else{
					beep_close();
				}
			}
				break;
			case 3:{
				if(key_flag){
					fan_on();
				}else{
					fan_off();
				}
			}
				break;
			}
	}
}

