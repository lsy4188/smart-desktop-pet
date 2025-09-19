#include "key.h"
#include "systick.h"

#define KEY2 GPIO_Pin_8
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

void init_key_INT(void){
GPIO_InitTypeDef GPIO_InitStruct;
EXTI_InitTypeDef EXTI_InitStruct;
NVIC_InitTypeDef NVIC_InitStruct;
//开启GPIO时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//开启系统时钟(stm32f4xx_syscfg.c)
RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG , ENABLE);

//GPIO初始化
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
GPIO_InitStruct.GPIO_Pin=KEY2;
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
GPIO_Init(GPIOC, &GPIO_InitStruct);

//选择用作EXTI线的GPIO引脚
SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource8);

//EXTI外部中断初始化
EXTI_InitStruct.EXTI_Line=EXTI_Line8;//开启外部中断线8
EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;//中断模式
EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;//上升沿触发
EXTI_InitStruct.EXTI_LineCmd=ENABLE;//开启EXTI外部中断
EXTI_Init(&EXTI_InitStruct);

//嵌套向量优先级组配置
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//嵌套向量中断控制器初始化
NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//设置抢占优先级(数值越小优先级越大
NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//设置响应优先级
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//开启NVIC
NVIC_Init(&NVIC_InitStruct);

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


u8 get_key_val_same(u8 num){
u8 res=0;
if (!GPIO_ReadInputDataBit(GPIOC, KEY3)){
	systick_ms(20);
	if (!GPIO_ReadInputDataBit(GPIOC, KEY3)){
		while(!GPIO_ReadInputDataBit(GPIOC, KEY3));
		key_flag=~key_flag;
	if(key_flag){
		mode++;
		if (mode > num) {
          mode = 1;  
        }
	}
		res=1;
		return res;
	}
}
return res;
}



