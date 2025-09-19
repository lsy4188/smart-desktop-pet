#include "usart.h"


/*
PG9->USART6_RX   
PG14->USART6_TX
*/
void init_usart6(void){
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
/*USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
USART_ITConfig(USART6,USART_IT_IDLE , ENABLE);//判断数据结束*/


//NVIC配置
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//当有多个中断源的时候需要配置
/*NVIC_InitStruct.NVIC_IRQChannel=USART6_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级
NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;//响应优先级
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitStruct);*/

//开启USART
USART_Cmd(USART6, ENABLE);

}
/*
PB10->USART3_TX
PB11->USART3_RX
*/

void init_usart3(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStruct);
	
	USART_ClearFlag(USART3,USART_FLAG_TC);
	
	//USART_ITConfig(USART3,USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
		
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART3, ENABLE);
}

/*
PA2->USART2_TX
PA3->USART2_RX
*/

void init_usart2(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,GPIO_AF_USART2);

	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);

	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE,ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=4;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART2,ENABLE);
}

extern u8 cache[32];
extern u8 cmd_ready;
extern u8 cmd_index;
extern u8 mode;
extern u8 key_flag;
void parse_command(void){
	cache[31]='\0';
	if(strcmp((char*)cache,"led")==0){
	mode=LED;
	key_flag^=0x01;
	}else if(strcmp((char*)cache,"beep")==0){
	mode=BEEP;	
	key_flag^=0x02;
	}else if(strcmp((char*)cache,"fan")==0){
	mode=FAN;
	key_flag^=0x04;
	}else{
		send_string(USART6,"UNKNOW COMMAND!!!");
	}
	cmd_index=0;
	cmd_ready=0;
	memset(cache,0,sizeof(cache));
}


void send_string(USART_TypeDef * USARTx,const char *str){
	int length=strlen(str);
	while(*str){
		USART_SendData(USARTx, (u8)*str++);
		while (!USART_GetFlagStatus(USARTx,USART_FLAG_TC));
		USART_ClearFlag(USARTx,USART_FLAG_TC);
	}
}


int fputc(int ch,FILE *fp){
USART_SendData(USART6, ch);
while (!USART_GetFlagStatus(USART6,USART_FLAG_TC));
USART_ClearFlag(USART6,USART_FLAG_TC);
return ch;
}


