#include "systick.h"
#include "led.h"
#include "key.h"
int main(void)
{
	init_led();
	init_systick(168);
	init_key();
	while(1){ 
		if(get_key_val()){
			turn_on_ledx(LED1);
		//	systick_ms(1000);
		}else{
			turn_off_ledx(LED1);
		}
	}
}

