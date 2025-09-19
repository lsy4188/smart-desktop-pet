#include "systick.h"
#include "led.h"
#include "key.h"
#include "beep.h"
u8 key_flag=0;
u8 mode=0;
int main(void)
{
	init_led();
	init_systick(168);
	init_key();
	init_beep();
	while(1){ 
		get_key_val();
		if(key_flag&&mode==1){
			turn_on_ledx(LED1);
		}else if(key_flag&&mode==2){
			beep_open();
		}else{
			turn_off_ledx(LED1);
			beep_close();
		}
	}
}

