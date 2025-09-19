#include "systick.h"
#include "led.h"
#include "key.h"
#include "beep.h"
int main(void)
{
	init_led();
	init_systick(168);
	init_key();
	init_beep();
	while(1){ 
		u8 temp=get_key_val();
		if(temp==1){
			turn_on_ledx(LED1);
		}else if(temp==2){
			beep_open();
		}
		else{
			turn_off_ledx(LED1);
		}
	}
}

