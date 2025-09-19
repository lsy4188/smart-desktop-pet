#include "key.h"
#include "systick.h"
#define KEY3 GPIO_Pin_5
#define KEY4 GPIO_Pin_9

/*
KEY1->PC9
KEY2->PC8
KEY3->PC5
USER->KEY4->PF9
*/
void init_key(void){
//GPIOC
GPIO_InitTypeDef GPIO_InitStruct;
//初始化时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;//输入模式
GPIO_InitStruct.GPIO_Pin=KEY3;//引脚
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//上拉
GPIO_Init(GPIOC, &GPIO_InitStruct);
//GPIOF
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
GPIO_InitStruct.GPIO_Pin=KEY4;//引脚
GPIO_Init(GPIOF, &GPIO_InitStruct);
}
/*
不同按键多个功能
*/
extern u8 key_flag;
extern u8 mode;
u8 get_key_val_diff(void){
u8 val=0;
if(!GPIO_ReadInputDataBit(GPIOC, KEY3)){
	systick_ms(20);//按键消抖
	if(!GPIO_ReadInputDataBit(GPIOC, KEY3)){
		while(!GPIO_ReadInputDataBit(GPIOC, KEY3));//长按
		key_flag=~key_flag;
		mode=1;
		val=1;
		return val;
	}
}
if(!GPIO_ReadInputDataBit(GPIOF, KEY4)){
	systick_ms(20);//按键消抖
	if(!GPIO_ReadInputDataBit(GPIOF, KEY4)){
		while(!GPIO_ReadInputDataBit(GPIOF, KEY4));//长按
		key_flag=~key_flag;
		mode=2;
		val=1;
		return val;
	}
}
return val;
}

/*
同一按键多个功能 
*/ 
u8 press_cnt=0;
u8 get_key_val_same(u8 num){
	u8 res=0;
press_cnt++;
if (!GPIO_ReadInputDataBit(GPIOC, KEY3)){
	systick_ms(20);
	if (!GPIO_ReadInputDataBit(GPIOC, KEY3)){
		while(!GPIO_ReadInputDataBit(GPIOC, KEY3));
		key_flag=~key_flag;
	if(key_flag){
		mode++;
	}
	if(press_cnt>num*2){
		mode=0;
	}
		res=1;
		return res;
	}
}
return res;
}



