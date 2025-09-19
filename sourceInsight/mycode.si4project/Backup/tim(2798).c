#include "tim.h"

void init_tim_basic(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
	//开启片内外设定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//初始化片内外设定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period=5000-1;//1400-1;//装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;//60000-1;//预分频
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	/*//开启定时器中断(不使用中断实现基本延时)
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	//初始化嵌套向量中断控制器
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;//设置中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	//启动片内外设定时器
	//TIM_Cmd(TIM6, ENABLE);*/
	TIM_Cmd(TIM6, DISABLE);
}

void delay_ms(uint8_t ms){
	while(ms--){
		delay_us(1000);
	}
}

void delay_us(uint8_t us){
	if(us<=0) return;
	TIM_SetCounter(TIM6, 0);//清除计数器
	TIM_SetAutoreload(TIM6, us);//重新设置重装载值
	TIM_Cmd(TIM6, ENABLE);
	while(!TIM_GetFlagStatus(TIM6, TIM_FLAG_Update));
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, DISABLE);

}

/*
输出比较
PA15-->beep-->TIM2
PC6-->fan-->TIM3
PF8-->LED-->TIM13
PA0(TIM5)--PC6
*/
void init_tim_common(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//配置GPIO
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//配置AFIO
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);
	//定时器配置
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=5000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	//初始化输出比较通道
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//输出的模式为PWM1
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//输出极性为高(根据电路图得来)
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
	TIM_OCInitStruct.TIM_Pulse=0;//比较初值
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);
	TIM_ARRPreloadConfig(TIM3, ENABLE);//开启装载的影子寄存器
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//开启比较的影子寄存器
	TIM_CtrlPWMOutputs(TIM3, ENABLE);//使能PWM输出
	//启动定时器
	TIM_Cmd(TIM3, ENABLE);
}



