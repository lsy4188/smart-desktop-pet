#include "key.h"
#include "systick.h"
#define KEY3 GPIO_Pin_5
/*
KEY1->PC9
KEY2->PC8
KEY3->PC5
USER->KEY4->PF9
*/
void init_key(void){
GPIO_InitTypeDef GPIO_InitStruct;
//初始化时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;//输入模式
GPIO_InitStruct.GPIO_Pin=KEY3;//引脚
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//上拉
GPIO_Init(GPIOC, &GPIO_InitStruct);
}

u8 get_key_val(void){
u8 val=0;
if(!GPIO_ReadInputDataBit(GPIOC, KEY3)){
	systick_ms(20);//按键消抖
	if(!GPIO_ReadInputDataBit(GPIOC, KEY3)){
		val=1;
		return val;
	}
}
return val;
}


