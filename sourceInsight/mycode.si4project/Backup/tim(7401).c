#include "tim.h"

void init_tim_basic(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//开启片内外设定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//初始化片内外设定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period=1400-1;//装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler=60000-1;//预分频
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	//开启定时器中断
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	//启动片内外设定时器
	TIM_Cmd(TIM6, ENABLE);

	//初始化嵌套向量中断控制器
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;//设置中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}
/*
PA15-->beep-->TIM2
PC6-->fan-->TIM8
PF8-->LED-->TIM13
*/
void init_tim_common(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	//配置GPIO
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	//配置AFIO
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);
	//定时器配置
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=5000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStruct);
	//初始化输出比较通道
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//输出的模式为PWM1
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_Low;//输出极性为高(根据电路图得来)
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
	TIM_OCInitStruct.TIM_Pulse=0;//比较初值
	TIM_OC1Init(TIM13, &TIM_OCInitStruct);
	TIM_ARRPreloadConfig(TIM13, ENABLE);//开启装载的影子寄存器
	TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);//开启比较的影子寄存器
	TIM_CtrlPWMOutputs(TIM13, ENABLE);//使能PWM输出
	//启动定时器
	TIM_Cmd(TIM13, ENABLE);
}



