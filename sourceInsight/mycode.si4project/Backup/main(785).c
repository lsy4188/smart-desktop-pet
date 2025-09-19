#include "systick.h"
#include "led.h"
int main(void)
{
    init_led();
	init_systick(168);
	while(1){ 
		systick_ms(1000);
		turn_on_ledx(LED1);
		systick_ms(1000);
		turn_off_ledx(LED1);
		turn_on_ledx(LED2);
		systick_ms(1000);
		turn_off_ledx(LED2);
		turn_on_ledx(LED3);
		systick_ms(1000);
		turn_off_ledx(LED3);
	}
}

