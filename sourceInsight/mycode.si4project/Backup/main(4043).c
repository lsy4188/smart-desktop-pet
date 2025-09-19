#include "systick.h"
#include "led.h"
int main(void)
{
    init_led();
	init_systick();
	while(1){ 
	}
}

