#include "i2c.h"

#define I2C1_SCL GPIO_Pin_6
#define I2C1_SDA GPIO_Pin_7
/*
PB6-->I2C1_SCL
PB7--> I2C1_SDA
*/
void init_i2c(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//GPIO配置 
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//开启复用功能
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;//开漏模式
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//上拉模式
	GPIO_InitStruct.GPIO_Pin= I2C1_SDA;//引脚初始化
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//设置传输频率
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//复用功能配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);

	//I2C初始化、中断和启动
	I2C_InitStruct.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed=100;//标准模式
	//I2C_InitStruct.I2C_DutyCycle=;快速模式下需要指定
	I2C_InitStruct.I2C_Mode=I2C_Mode_SMBusHost;
	I2C_InitStruct.I2C_OwnAddress1=0x57;
	I2C_Init(I2C1, &I2C_InitStruct);

	//中断配置
	I2C_ITConfig(I2C1,I2C_IT_RXNE, ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel=I2C1_EV_IRQn;//选择事件中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=4;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//开启中断
	NVIC_Init(&NVIC_InitStruct);
	
	I2C_Cmd(I2C1, FunctionalState NewState);
	
}


