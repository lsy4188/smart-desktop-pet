#include "led.h"
#include "delay.h"
int main(void)
{
	init_led();
	
	while(1){ 
		turn_on_ledx(LED1);
		delay_ms(1000);
		turn_off_ledx(LED1);
	}
}

