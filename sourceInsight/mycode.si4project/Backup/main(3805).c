#include "systick.h"
#include "led.h"
#include "beep.h"
int main(void)
{
    init_beep();
	init_systick(168);
	while(1){ 
		systick_ms(1000);
		beep_open();
		turn_on_ledx(LED1);
		systick_ms(1000);
		beep_close();
		turn_off_ledx(LED1);
		turn_on_ledx(LED2);
		systick_ms(1000);
		turn_off_ledx(LED2);
		turn_on_ledx(LED3);
		systick_ms(1000);
		turn_off_ledx(LED3);
	}
}

