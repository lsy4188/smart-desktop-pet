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
uint32_t temp=0;
uint8_t systick_flg=0;
while(systick_flg == 0)
{
	if(ms > 798)
	{
		SysTick->LOAD = 798*time_ms;//延时时间需要计总次数
		ms=ms-798;
	}
	else
	{
		SysTick->LOAD = ms*time_ms;//延时时间需要计总次数
		systick_flg = 1;
	}
	SysTick->VAL = 0;//清空当前值寄存器
	SysTick->CTRL |= 0x01;//开启定时器
	do
	{
		temp = SysTick->CTRL;//获取控制状态寄存器的值
	}
	while(!(temp & (0x01<<16)));//判断定时时间是否到
	SysTick->CTRL &= ~0x01;//关闭定时器

	}

}

