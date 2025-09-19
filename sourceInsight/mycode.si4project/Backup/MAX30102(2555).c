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

uint8_t Get_FIFO_SampleCount(void){
	uint8_t wr_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
    uint8_t rd_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
    
    // 计算可读样本数（考虑循环FIFO）
    int samples = (wr_ptr - rd_ptr) & 0x1F;
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
	return i2c_read(MAX30102_I2C_ADDR<<1|0x01, reg);
}

void Write_MAX30102_Reg(uint8_t reg, uint8_t value){
	i2c_write(MAX30102_I2C_ADDR<<1, reg, value);
}

/************************信号处理*********************************/
void init_SignalProcess(SignalProcess *sp){
	memset(sp,0,sizeof(SignalProcess));
	sp->ac_red=0.0f;
	sp->dc_ir=0.0f;

}

void Process_Signal(SignalProcess *sp, uint32_t red, uint32_t ir){
     // 手指检测：IR直流分量阈值判断（根据实际调试调整）
    const uint32_t IR_THRESHOLD = 50000; // 无手指时通常<10000
    sp->finger_detected = (ir > IR_THRESHOLD) ? 1 : 0;
    
    // 仅当有手指时更新信号处理
    if (sp->finger_detected) {
        // 更新缓冲区
        sp->red_buffer[sp->index] = red;
        sp->ir_buffer[sp->index] = ir;
        sp->index = (sp->index + 1) % 100;
        
        // 计算DC分量 (移动平均)
        sp->dc_red = 0.95f * sp->dc_red + 0.05f * red;
        sp->dc_ir = 0.95f * sp->dc_ir + 0.05f * ir;
        
        // 计算AC分量 (当前值 - DC)
        sp->ac_red = red - sp->dc_red;
        sp->ac_ir = ir - sp->dc_ir;
    } else {
        // 手指离开时重置缓冲区
        memset(sp->red_buffer, 0, sizeof(sp->red_buffer));
        memset(sp->ir_buffer, 0, sizeof(sp->ir_buffer));
        sp->dc_red = sp->dc_ir = sp->ac_red = sp->ac_ir = 0.0f;
    }
}

// 计算心率(HR)和血氧饱和度(SpO2)
void Calculate_HR_SpO2(SignalProcess *sp, float *hr, float *spo2){
	int i=0;
	float val,red_amp,R ;
    float max_red = 0;
    float min_red = 999999;
	if (!sp->finger_detected) {
        *hr = 0.0f;
        *spo2 = 0.0f;
        return;
    }
	 // 1. 寻找红光AC分量的峰值
    for(i=0; i<100; i++){
        val = sp->red_buffer[i] - sp->dc_red;
        if(val > max_red) max_red = val;
        if(val < min_red) min_red = val;
    }  
    // 2. 计算红光AC分量幅度
    red_amp = max_red - min_red;
    // 3. 计算心率 (简化算法)
    *hr = 60.0f * 400.0f / 100.0f; // 400Hz采样率，100个样本 
    // 4. 计算R值 (用于SpO2)：R = (红光AC/红光DC)/(红外AC/红外DC)
    R = (sp->ac_red / sp->dc_red) / (sp->ac_ir / sp->dc_ir);
    // 5. 计算SpO2 (经验公式)
    *spo2 = 110.0f - 25.0f * R;
    if(*spo2 < 70.0f) *spo2 = 70.0f;
    if(*spo2 > 100.0f) *spo2 = 100.0f;
}



