#include "systick.h"
#include "led.h"

void init_systick(void){
RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
RCC_PLLCmd(ENABLE);
while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);
RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
while (RCC_GetSYSCLKSource()!=0x08);
SystemCoreClockUpdate();
}

u16 cnt=0;
void SysTick_Handler(void){
	cnt++;
	if(cnt==1000){
		toggle_ledx(LED1);
		cnt=0;
	}
}


