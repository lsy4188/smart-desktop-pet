#include "systick.h"
#include "led.h"
#include "key.h"
int main(void)
{
	init_led();
	init_systick(168);
	
	while(1){ 
		if(get_key_val()){
			turn_on_ledx(LED1);
		}else{
			turn_off_ledx(LED1);
		}
	}
}

