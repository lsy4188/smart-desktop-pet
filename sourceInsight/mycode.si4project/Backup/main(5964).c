#include "led.h"
int main(void)
{
	init_led();
	while(1){ 
		turn_on_all_led();
	}
}
