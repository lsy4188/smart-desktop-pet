#include "MAX30102.h"

void init_MAX30102(void){
	//初始化i2c
	init_i2c();
	//复位
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x40);
	//等待复位完成
	while(Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG)& 0x40);
	//配置FIFO：采样平均=4，回滚使能，FIFO满阈=32
	Write_MAX30102_Reg(MAX30102_REG_FIFO_CONFIG,0x5F);
	// 模式配置：SpO2模式
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x03);
	// SPO2配置：采样率100Hz，LED脉冲宽度=411μs (18位分辨率)
	Write_MAX30102_Reg(MAX30102_REG_SPO2_CONFIG,0x27);
	// LED脉冲幅度：红光=24.8mA，红外光=24.8mA
    Write_MAX30102_Reg(MAX30102_REG_LED1_PA, 0x7C); 
    Write_MAX30102_Reg(MAX30102_REG_LED2_PA, 0x7C); 
    // 多LED控制：启用LED1和LED2
    Write_MAX30102_Reg(MAX30102_REG_MULTI_LED1, 0x21);
	 // 使能新数据就绪中断和温度就绪中断
    Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE1, 0x40);
	Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE2, 0x02);
    // 温度测量配置
    Write_MAX30102_Reg(MAX30102_REG_TEMP_CONFIG, 0x01);
}

void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir){
	int i;
	uint8_t data[6];
	for(i=0;i<6;i++){
		data[i]=Read_MAX30102_Reg(MAX30102_REG_FIFO_DATA);
	}
	 // 组合数据 (18位有效数据)
    *red = ((data[0] << 16) | (data[1] << 8) | data[2]) >> 6;
    *ir = ((data[3] << 16) | (data[4] << 8) | data[5]) >> 6;

}

float Read_MAX30102_Temperature(void){
	uint8_t temp_int=0;
	uint8_t temp_fra=0;
	// 温度测量配置
    Write_MAX30102_Reg(MAX30102_REG_TEMP_CONFIG, 0x01);
	//等待温度就绪
	while(!Read_MAX30102_Reg(MAX30102_REG_INT_STATUS2&0x02));
	temp_int=Read_MAX30102_Reg(MAX30102_REG_TEMP_INTG);
	temp_fra=Read_MAX30102_Reg(MAX30102_REG_TEMP_FRAC);
	return temp_int+(temp_fra * 0.0625f); // 每步0.0625°C
}

uint8_t Get_MAX30102_PartID(void){
	return Read_MAX30102_Reg(MAX30102_REG_PART_ID);
}

uint8_t Get_MAX30102_RevID(void){
	return Read_MAX30102_Reg(MAX30102_REG_REV_ID);

}

void Reset_MAX30102(void){
	uint32_t timeout = 1000000;
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, 0x40);
    while ((Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG) & 0x40) && timeout--);

}

void Start_MAX30102_Sampling(void){
	uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode & ~0x70); // 清除关机位
}

void Stop_MAX30102_Sampling(void){
    uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode | 0x40); // 设置关机位

}

uint8_t Read_MAX30102_Reg(uint8_t reg){
	return i2c_read(MAX30102_I2C_ADDR, reg);
}

void Write_MAX30102_Reg(uint8_t reg, uint8_t value){
	i2c_write(MAX30102_I2C_ADDR, reg, value);
}

void get_Data(void){
	uint32_t red;
	uint32_t ir;
	float tem;
	Start_MAX30102_Sampling();
	if(Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1)&0x40){
		Read_MAX30102_FIFO(&red,&ir);
	}
	tem=Read_MAX30102_Temperature();
	printf("red=%d   ir=%d   tem=%.2f\n",red,ir,tem);
}




