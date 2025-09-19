#include "systick.h"
#include "led.h"

u16 time_us=0;
u16 time_ms=0;
void init_systick(u16 systick){
SysTick->CTRL &= ~0x05;//关闭定时器，时钟8分频

time_us = systick/8;//计时1us的时间需要计次数
time_ms = 1000*time_us;//计时1ms的时间需要计的次数	

}

void systick_us(u16 us){
	uint32_t temp=0;	
	SysTick->LOAD = us*time_us;//延时时间需要计总次数
	SysTick->VAL = 0;//清空当前值寄存器
	SysTick->CTRL |= 0x01;//开启定时器
	do
	{
		temp = SysTick->CTRL;//获取控制状态寄存器的值
	}
	while(!(temp & (0x01<<16)));//判断定时时间是否到
	SysTick->CTRL &= ~0x01;//关闭定时器	
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

