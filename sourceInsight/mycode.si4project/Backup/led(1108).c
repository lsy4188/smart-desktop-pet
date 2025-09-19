#include "led.h"
/*
led1-->PC4
led2-->PC3
led3-->PC13
*/
void init_led(void){
GPIO_InitTypeDef GPIO_InitStruct;
//开启时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;//输出模式
GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽模式
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//上拉电阻
GPIO_InitStruct.GPIO_Speed=GPIO_Low_Speed;//低频
//初始化GPIO(PC4)
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;//4引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);
//初始化GPIO(PC3)
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;//3引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);
//初始化GPIO(PC13)
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;//13引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);
GPIO_SetBits(GPIOC, GPIO_Pin_4);
GPIO_SetBits(GPIOC, GPIO_Pin_3);
GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void turn_off_all_led(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void turn_on_all_led(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}



