#include "led.h"
/*
led1-->PC4
led2-->PC3
led3-->PC13
*/
void init_led(void){
/*
使用标准库
GPIO_InitTypeDef GPIO_InitStruct;
//开启时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;//输出模式
GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽模式
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//上拉电阻
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;//低频
//初始化GPIO(PC4)
GPIO_InitStruct.GPIO_Pin=LED1;//4引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);
//初始化GPIO(PC3)
GPIO_InitStruct.GPIO_Pin=LED2;//3引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);

//初始化GPIO(PC13)
GPIO_InitStruct.GPIO_Pin=LED3;//13引脚
GPIO_Init(GPIOC, &GPIO_InitStruct);
turn_off_all_led();*/

//使用寄存器
//开启时钟
RCC->APB1ENR |=0x04;
GPIOC->MODER &=~(0x03<<4);
GPIOC->MODER |=0x01<<4;
GPIOC->OTYPER |=0x01<<4;
GPIOC->OSPEEDR |=0x01<<4;
GPIOC->PUPDR |=0x01<<4;
GPIOC->ODR &=~(0x01<<4);

}

void turn_on_all_led(void){
	GPIOC->ODR |=0x01<<4;
	/*GPIO_SetBits(GPIOC, LED1);
	GPIO_SetBits(GPIOC, LED2);
	GPIO_SetBits(GPIOC, LED3);*/
}

void turn_off_all_led(void){
	GPIOC->ODR &=~(0x01<<4);
	/*
	GPIO_ResetBits(GPIOC, LED1);
	GPIO_ResetBits(GPIOC, LED2);
	GPIO_ResetBits(GPIOC, LED3);*/
}

void turn_on_ledx(u16 led){
	GPIO_SetBits(GPIOC, led);
}

void turn_off_ledx(u16 led){
	GPIO_ResetBits(GPIOC, led);
}


