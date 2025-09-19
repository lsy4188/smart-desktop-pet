#include "i2c.h"

/*
PB6-->I2C1_SCL
PB7--> I2C1_SDA
*/
void init_i2c(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	//时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//GPIO配置 
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//开启复用功能
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;//开漏模式
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//浮空模式
	GPIO_InitStruct.GPIO_Pin= I2C1_SDA|I2C1_SCL;//引脚初始化
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//设置传输频率
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//复用功能配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);

	//I2C初始化和启动
	I2C_InitStruct.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed=I2C1_CLOCK_SPEED;//标准模式
	//I2C_InitStruct.I2C_DutyCycle=I2C_DutyCycle_2;快速模式下的占空比，标准模式不需要
	I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;//标准I2C模式
	I2C_InitStruct.I2C_OwnAddress1=0x00;//主设备自己的地址，可以任意（不冲突即可）
	I2C_Init(I2C1, &I2C_InitStruct);
	I2C_Cmd(I2C1, ENABLE);
	
}


void i2c_write(uint8_t address,uint8_t reg,uint8_t data){
	//判断总线是否空闲
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	//发送起始条件
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	//发送从设备地址
	I2C_Send7bitAddress(I2C1,address<<1,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//发送寄存器地址
	if(reg!=0){
	I2C_SendData(I2C1,reg);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	}
	//发送具体数据
	I2C_SendData(I2C1,data);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//结束发送
	I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t i2c_read(uint8_t address,uint8_t reg){
	uint8_t data=0;
	//判断总线是否空闲
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	//发送起始条件(写模式)
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	//发送从设备地址(写模式)
	I2C_Send7bitAddress(I2C1,address<<1,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//发送寄存器地址(写模式)
	I2C_SendData(I2C1,reg);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//发送起始条件(读模式)
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	//发送从设备地址(读模式)
	I2C_Send7bitAddress(I2C1,address<<1|0x01,I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	//硬件会自动触发 ACK/NACK 的发送
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	//结束发送(最后一个字节的 NACK 和 STOP 必须同步)
	I2C_GenerateSTOP(I2C1, ENABLE);
	//读取数据(只读取一个字节)
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
	data=I2C_ReceiveData(I2C1);
	//重新开启ACK确认机制
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	return data;
}


/*********************************自定义i2c功能*******************************/

void init_i2c_cust(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void init_i2c_in(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void init_i2c_out(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void start_i2c_cust(void){
	SCL_L;
	init_i2c_out();
	SDA_H;
	SCL_H;
	systick_us(5);
	SDA_L;
	systick_us(5);
	SCL_L;
}
void stop_i2c_cust(void){
	SCL_L;
	init_i2c_out();
	SDA_L;
	SCL_H;
	systick_us(5);
	SDA_H;
	systick_us(5);
}
void send_ACK_cust(void){
	SCL_L;
	init_i2c_out();
	SDA_L;
	SCL_H;
	systick_us(5);
	SCL_L;

}
void send_NACK_cust(void){
	SCL_L;
	init_i2c_out();
	SDA_H;
	SCL_H;
	systick_us(5);
	SCL_L;
}
uint8_t wait_ACK_cust(void){
	uint16_t time=10000;
	SCL_L;
	init_i2c_in();
	SCL_H;
	while(SDA_READ||time--==0){
		stop_i2c_cust();
		return NACK;
	}
	SCL_L;
	return ACK;
}

uint8_t read_data_cust(uint8_t flg){
	int i=0;
	uint8_t data=0;
	SCL_L;
	init_i2c_in();
	for(i=0;i<8;i++){
		data=data<<1;
		SCL_H;
		if(SDA_READ){
			data=data|0x01;
		}
		systick_us(1);
		SCL_L;
		systick_us(1);
	}
	if(!flg){
		send_ACK_cust();
	}else{
		send_NACK_cust();
	}
	SCL_L;
	return data;
}
void write_data_cust(uint8_t data){
	int i=0;
	SCL_L;
	init_i2c_out();
	for(i=0;i<8;i++){
		if(data&0x80){
			SDA_H;
		}else{
			SDA_L;
		}
		SCL_H;
		systick_us(1);
		SCL_L;
		systick_us(1);
		data=data<<1;
	}
}




