#include "tim.h"

void init_tim(void){
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

