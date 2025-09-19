#include "key.h"

#define C_KEY1 GPIO_Pin_9
#define C_KEY2 GPIO_Pin_8
#define C_KEY3 GPIO_Pin_5
#define F_KEY4 GPIO_Pin_9

KEY key_which=DEFALUT;

/*
KEY1->PC9
KEY2->PC8
KEY3->PC5
USER->F_KEY4->PF9
*/
void init_key(void){
//GPIOC
GPIO_InitTypeDef GPIO_InitStruct;
//初始化时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;//输入模式
GPIO_InitStruct.GPIO_Pin=C_KEY3;//引脚
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//上拉
GPIO_Init(GPIOC, &GPIO_InitStruct);
//GPIOF
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
GPIO_InitStruct.GPIO_Pin=F_KEY4;//引脚
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
GPIO_InitStruct.GPIO_Pin=C_KEY1 | C_KEY2 | C_KEY3;
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
GPIO_Init(GPIOC, &GPIO_InitStruct);

//选择用作EXTI线的GPIO引脚(不能同时配置多个引脚源)
SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource5);
SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource8);
SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource9);


//EXTI外部中断初始化
EXTI_InitStruct.EXTI_Line=EXTI_Line5|EXTI_Line8|EXTI_Line9;//开启外部中断线
EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;//中断模式
EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿触发
EXTI_InitStruct.EXTI_LineCmd=ENABLE;//开启EXTI外部中断
EXTI_Init(&EXTI_InitStruct);

//嵌套向量优先级组配置
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//当有多个中断源的时候需要配置

//嵌套向量中断控制器初始化
NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//设置抢占优先级(数值越小优先级越大
NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//设置响应优先级
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//开启NVIC
NVIC_Init(&NVIC_InitStruct);

}

uint8_t detect_key(void){
	uint8_t key;
	if(key_which==KEY1){
		key=KEY1;
	}else if(key_which==KEY2){
		key=KEY2;
	}else if(key_which==KEY3){
		key=KEY3;
	}else{
		key=DEFALUT;
	}
	key_which=DEFALUT;
	return key;
}

