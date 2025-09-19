#include "fan.h"

void init_fan(void){
GPIO_InitTypeDef GPIO_InitStruct;
//开启GPIO时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;//输出模式
GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽模式
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//上拉电阻
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;//低频
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;//6引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);
GPIO_ResetBits(GPIOC, GPIO_Pin_6);
}

void fan_on(void){
GPIO_SetBits(GPIOC, GPIO_Pin_6);
}

void fan_off(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
}


