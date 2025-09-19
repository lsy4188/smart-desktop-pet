#include "MAX30102.h"

void init_MAX30102(void){
	//初始化i2c
	init_i2c();
	//复位
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x40);
	//等待复位完成
	while (Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG)& 0x40);
	//配置FIFO
	Write_MAX30102_Reg(MAX30102_REG_FIFO_CONFIG,0x4F);
	//模式配置
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x03);
	//SPO2配置
	Write_MAX30102_Reg(MAX30102_REG_SPO2_CONFIG,0x27);
	//LED脉冲幅度
   Write_MAX30102_Reg(MAX30102_REG_LED1_PA, 0x24); 
   Write_MAX30102_Reg(MAX30102_REG_LED2_PA, 0x24); 
    //多LED控制
   Write_MAX30102_Reg(MAX30102_REG_MULTI_LED, 0x21);
	// 使能FIFO满中断
   Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE, 0x80);

}

void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir){
	int i;
	uint8_t data[6];
	for(i=0;i<6;i++){
		data[i]=Read_MAX30102_Reg(MAX30102_REG_FIFO_DATA);
	}
	 // 组合数据 (18位有效数据)
    *red = ((data[0] << 16) | (data[1] << 8) | data[2]) & 0x03FFFF;
    *ir = ((data[3] << 16) | (data[4] << 8) | data[5]) & 0x03FFFF;
}

uint8_t Get_FIFO_SampleCount(void){
	uint8_t wr_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
    uint8_t rd_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
    
    // 计算可读样本数（考虑循环FIFO）
    int samples = (wr_ptr - rd_ptr) & 0x0F;
    if (samples < 0) samples += 16;
    
    return (uint8_t)samples;
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


