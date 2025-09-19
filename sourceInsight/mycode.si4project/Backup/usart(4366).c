#include "usart.h"
#include "key.h"
#include "stdio.h"
#include "string.h"




/*
PG9->USART6_RX   
PG14->USART6_TX
*/
void init_usart(void){
GPIO_InitTypeDef GPIO_InitStruct;
USART_InitTypeDef USART_InitStruct;
NVIC_InitTypeDef NVIC_InitStruct;


//开GPIO时钟
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
//开启USART时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);

//初始化GPIO
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//复用功能
GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_14;
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//上拉
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
GPIO_Init(GPIOG,&GPIO_InitStruct);

//配置具体复用的功能(AFIO)
GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);

//usart初始化
USART_InitStruct.USART_BaudRate=115200;//波特率
USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件流控制器
USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//收或发模式设置
USART_InitStruct.USART_Parity=USART_Parity_No; //奇偶校验
USART_InitStruct.USART_StopBits=USART_StopBits_1; //停止位
USART_InitStruct.USART_WordLength=USART_WordLength_8b;//字长
USART_Init(USART6, &USART_InitStruct);
USART_ClearFlag(USART6, USART_FLAG_TC);//清空发送完成标志，否则会出现第一个字母丢失

//usart中断配置（接收中断）
USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

//NVIC配置
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//当有多个中断源的时候需要配置
NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitStruct);

//开启USART
USART_Cmd(USART6, ENABLE);

}

extern u16 cache[32];
extern u16 ready;
extern u16 cmd_index;
extern u8 mode;
extern u8 key_flag;
void ParseCommand(void){
	cache[31]='\0';
	if(strcmp((char*)cache,"led_open")==0){
	mode=LED;
	key_flag^=0x01;
	}else if(strcmp((char*)cache,"beep_open")==0){
	mode=BEEP;	
	key_flag^=0x02;
	}else if(strcmp((char*)cache,"fan_open")==0){
	mode=FAN;
	key_flag^=0x04;
	printf("1111");
	}else{
		USART_ClearFlag(USART6, USART_FLAG_TC);
		USART_SendData(USART6, 'A');
		while (!USART_GetFlagStatus(USART6,USART_FLAG_TC));
		USART_ClearFlag(USART6,USART_FLAG_TC);
	}
	cmd_index=0;
	ready=0;
}

int fputc(int ch,FILE *fp){
USART_SendData(USART6, ch);
while (!USART_GetFlagStatus(USART6,USART_FLAG_TC));
USART_ClearFlag(USART6,USART_FLAG_TC);
return ch;
}

