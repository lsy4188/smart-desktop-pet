#include "MAX30102.h"

// 算法结果变量
int32_t heart_rate;
int8_t heart_rate_valid;
int32_t spo2;
int8_t spo2_valid;

uint32_t red_buffer[BUFFER_SIZE];
uint32_t ir_buffer[BUFFER_SIZE];
int buffer_index = 0;


void init_MAX30102(void){
	//初始化i2c
	init_i2c();
	//复位
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, 0x40);
	//FIFO指针清零
	Write_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
	Write_MAX30102_Reg(MAX30102_REG_OVF_COUNTER, 0x00);
	Write_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
	//配置FIFO：采样平均=4(平均过多会使得细节信号丢失),FIFO满阈=17
	Write_MAX30102_Reg(MAX30102_REG_FIFO_CONFIG,0x6F);
	// 模式配置：SpO2模式
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x03);
	// SPO2配置：设置 SpO2 传感器中 ADC 的满量程范围4096,采样率100Hz，LED脉冲宽度=411μs (18位分辨率)
	Write_MAX30102_Reg(MAX30102_REG_SPO2_CONFIG,0x27);
	// LED脉冲幅度：红光=6.2mA，红外光=6.2mA(电流过高可能光线太强导致光电二极管饱和（信号达到最大值））
     Write_MAX30102_Reg(MAX30102_REG_LED1_PA, 0x24); 
    Write_MAX30102_Reg(MAX30102_REG_LED2_PA, 0x24);
    // 多LED控制：启用LED1红光和LED2红外光
   	Write_MAX30102_Reg(MAX30102_REG_MULTI_LED1, 0x21);
	 // 使能满数据就绪中断，温度就绪中断
    Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE1, 0x80);
	Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE2, 0x02);
}
/*
判断样本数据是否已准备
*/
uint8_t MAX30102_samples_data_is_ready(void){
	if(Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1)&0x80){
		Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1);//清除中断
		return 1;
	}
	return 0;
}

/*
获取通道样本数据总量
注：采集速率过慢会导致FIFO一直是数据溢出状态
*/
uint16_t Get_MAX30102_samples(void){
	uint8_t wr_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
    uint8_t rd_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
    uint8_t ovf = Read_MAX30102_Reg(MAX30102_REG_OVF_COUNTER);
    if (ovf > 0) {	
        	Write_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
        	Write_MAX30102_Reg(MAX30102_REG_OVF_COUNTER, 0x00);
        	Write_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
			return 0;
    }
	return (wr_ptr >= rd_ptr) ? (wr_ptr - rd_ptr) : (32 - rd_ptr + wr_ptr);
}
/*
读取FIFO中的红外和红光数据
*/
void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir){
    uint8_t data[6];  
    // 读取FIFO数据（一次读取6字节：红光3字节 + 红外3字节）
    Read_MAX30102_Reg_BYTES(MAX30102_REG_FIFO_DATA, data, 6); 
    // 组合18位数据
    *red = ((data[0] << 16) | (data[1] << 8) | data[2]) >> 6;
    *ir  = ((data[3] << 16) | (data[4] << 8) | data[5]) >> 6;    
    // 确保只保留18位有效数据
    *red &= 0x03FFFF;
    *ir  &= 0x03FFFF;
	//printf("red=%d   ir=%d\n",*red,*ir);
}

/*
读取一组数据
*/
void Read_MAX30102_Reg_BYTES(uint8_t reg,uint8_t * data,uint8_t len){
	i2c_read_bytes(MAX30102_I2C_ADDR,reg, data, len);
}

/*
读取温度数据
*/
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

void Start_MAX30102_Sampling(void){
	uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode & ~0x80);
}

void Stop_MAX30102_Sampling(void){
    uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode | 0x80); // 设置关机位

}
/*
读取一个字节数据
*/
uint8_t Read_MAX30102_Reg(uint8_t reg){
	return i2c_read(MAX30102_I2C_ADDR, reg);
}

void Write_MAX30102_Reg(uint8_t reg, uint8_t value){
	i2c_write(MAX30102_I2C_ADDR, reg, value);
}


void get_real_SpO2(void){
	uint32_t red = 0, ir = 0;
	float temp;
	// 检查FIFO数据是否就绪
	if(MAX30102_samples_data_is_ready()){
		// 读取FIFO数据
		Read_MAX30102_FIFO(&red, &ir);
		if(buffer_index < BUFFER_SIZE){
			red_buffer[buffer_index] = red;
			ir_buffer[buffer_index] = ir;
			buffer_index++;
		//	printf("Sample %d: IR=%lu, RED=%lu\r\n", buffer_index, ir, red);
		}	
		if(buffer_index >= BUFFER_SIZE){
			maxim_heart_rate_and_oxygen_saturation(
				ir_buffer, BUFFER_SIZE, 
				red_buffer, 
				&spo2, &spo2_valid, 
				&heart_rate, &heart_rate_valid
			);
			if(heart_rate_valid && spo2_valid){
				printf("Heart Rate: %d bpm, SpO2: %d%%\r\n", heart_rate, spo2);
			}else if(heart_rate_valid){
				printf("Heart Rate: %d bpm, SpO2: INVALID\r\n", heart_rate);
			}else if(spo2_valid){
				printf("Heart Rate: INVALID, SpO2: %d%%\r\n", spo2);
			}else{
				printf("Measurement invalid. Check sensor placement.\r\n");
			}
			buffer_index = 0;
			temp = Read_MAX30102_Temperature();
			printf("Sensor Temperature: %.2f°C\r\n", temp);
		}
	}
	// 根据采样率延时（100Hz = 10ms）
	delay_ms(10);
}


