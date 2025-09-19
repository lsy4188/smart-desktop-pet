#include "systick.h"
#include "led.h"

u16 time_us=0;
u16 time_ms=0;
void init_systick(u16 systick){
SysTick->CTRL &=~SysTick_CTRL_CLKSOURCE_Msk;
time_us=systick/8;
time_ms=1000*time_us;
}

void systick_us(u16 us){
u32 temp=0;
SysTick->VAL=0;
SysTick->LOAD |= time_us*us;
SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
do
	{
	temp=SysTick->CTRL;
	}
while (!(temp&(0x01<<16)));
SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void systick_ms(u16 ms){
u32 temp=0;
SysTick->VAL=0;
SysTick->LOAD |= time_ms*ms;
SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
do
	{
	temp=SysTick->CTRL;
	}
while (!(temp&(0x01<<16)));
SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

