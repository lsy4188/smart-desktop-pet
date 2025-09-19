#include "systick.h"
#include "led.h"

u16 time_us=0;
u16 time_ms=0;
void init_systick(u16 systick){
SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;//关闭时钟源
time_us=systick/8;
time_ms=1000*time_us;
}

void systick_us(u16 us){
SysTick->VAL=0;
SysTick->LOAD |= time_us*us;
SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;
u16 flag=0;
do
	{
	flag=SysTick->CTRL;
	}
while (!(flag&(0x01<<16)));
SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

void systick_ms(u16 ms){
SysTick->VAL=0;
SysTick->LOAD |= time_ms*ms;
SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;
u16 flag=0;
do
	{
	flag=SysTick->CTRL;
	}
while (!(flag&(0x01<<16)));
SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

