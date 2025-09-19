#include "systick.h"
#include "led.h"

void init_systick(void){
SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;//AHB=168MHZ分频
SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;//使能中断	
SysTick->VAL=0;
SysTick->LOAD = (168000-1);
SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;
}
/*
void systick_us(u16 time_us){
SysTick->VAL=0;
SysTick->LOAD |= (168-1)*time_us;
SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;
}

void systick_us(u16 time_ms){
SysTick->VAL=0;
SysTick->LOAD |= (168000-1)*time_ms;
SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;
}*/

u16 cnt=0;
void SysTick_Handler(void){
	cnt++;
	if(cnt==1000){
		turn_on_ledx(LED1);
		cnt=0;
	}
}


