#include "spi.h"
/*
PB12-->SPI2_NSS(由标准 IO 端口驱动)
PB13-->SPI2_SCK
PB14-->SPI2_MISO
PB15-->SPI2_MOSI
*/
void init_spi(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	//初始化GPIO
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//配置复用功能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15, GPIO_AF_SPI2);
	//初始化SPI
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;//设置波特率分频器
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;//设置时钟相位(前沿触发)
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_High;//设置时钟极性(时钟空闲时输出高电平)
	SPI_InitStruct.SPI_CRCPolynomial=7;//循环冗余校验
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;//设置数据位数
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;//高位先行
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;//设置主机模式
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;//片选设置为软件触发
	SPI_Init(SPI2, &SPI_InitStruct);
	//开启SPI片内外设
	SPI_Cmd(SPI2, ENABLE);
	//设置NSS为高电平(暂不通讯)
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void send_data(uint8_t data){
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));//阻塞等待上一个数据发送完
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	SPI_I2S_SendData(SPI2, data);//发送数据
}

