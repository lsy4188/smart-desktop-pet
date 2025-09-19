#include "tim.h"

void init_tim6_basic(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//开启片内外设定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//初始化片内外设定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period=5000-1;//1400-1;//装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;//60000-1;//预分频
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM6, DISABLE);
}

void delay_ms(uint16_t ms){
	while(ms--){
		delay_us(1000);
	}
}

void delay_us(uint16_t us){
	if(us<=0) return;
	TIM_SetCounter(TIM6, 0);//清除计数器
	TIM_SetAutoreload(TIM6, us);//重新设置重装载值
	TIM_Cmd(TIM6, ENABLE);
	while(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)==RESET);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, DISABLE);

}

void init_tim7_basic(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//开启片内外设定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	//初始化片内外设定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period=1000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	//开启定时器中断(不使用中断实现基本延时)
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	//启动片内外设定时器
	TIM_Cmd(TIM7, ENABLE);

	//初始化嵌套向量中断控制器
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//通过设置嵌套向量优先级组从而实现RTOS的中断屏蔽
	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn;//设置中断线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=6;//1;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}


void init_tim2_common(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=1400-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=60000-1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	/*避免任务未创建成功就发起中断(写在了任务中)
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);*/
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
}

void init_tim4_common(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=100-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	TIM_Cmd(TIM4, ENABLE);
}



/*
输出比较
PA15-->beep-->TIM2
PC6-->fan-->TIM3	·
PF8-->LED-->TIM13
PA0(TIM5)--PC6
*/
void init_tim3_common(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//配置GPIO
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//配置AFIO
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);//PC6->TIM3_CH1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);//PC7->TIM3_CH2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);//PC8->TIM3_CH3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);//PC9->TIM3_CH4
	
	//定时器配置
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=20000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	//初始化输出比较通道1
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//输出的模式为PWM1
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//输出极性为高(根据电路图得来)(将TIM_OCInitStruct.TIM_OCPolarity换为TIM_OCInitStruct.TIM_OCNPolarity可能出现360度转-->待测试)
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
	TIM_OCInitStruct.TIM_Pulse=0;//比较初值
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//开启比较的影子寄存器
	//初始化输出比较通道2
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	//初始化输出比较通道3
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	//初始化输出比较通道4
	TIM_OC4Init(TIM3, &TIM_OCInitStruct);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);//开启装载的影子寄存器
	TIM_CtrlPWMOutputs(TIM3, ENABLE);//使能PWM输出
	//启动定时器
	TIM_Cmd(TIM3, ENABLE);
}






