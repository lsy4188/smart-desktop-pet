#include "MAX30102.h"

MAX30102 current_max30102 = SHUT_DOWN;

// 算法结果变量
int32_t heart_rate;
int8_t heart_rate_valid;
int32_t spo2;
int8_t spo2_valid;

uint32_t red_buffer[BUFFER_SIZE];
uint32_t ir_buffer[BUFFER_SIZE];
int buffer_index = 0;
uint32_t un_min, un_max, un_prev_data;  


void init_MAX30102(void){
	//初始化中断引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//初始化i2c(放到了freeRtos初始化位置)
	//init_i2c();
	//复位
	Reset_MAX30102();
	// 使能满数据就绪中断和新数据就绪中断
    Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE1, 0xc0);
	Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE2, 0x00);
	//FIFO指针清零
	Write_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
	Write_MAX30102_Reg(MAX30102_REG_OVF_COUNTER, 0x00);
	Write_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
	//配置FIFO：采样不平均(平均过多会使得细节信号丢失),FIFO满阈=17
	Write_MAX30102_Reg(MAX30102_REG_FIFO_CONFIG,0x0F);
	// 模式配置：SpO2模式
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x03);
	// SPO2配置：设置 SpO2 传感器中 ADC 的满量程范围4096,采样率100Hz，LED脉冲宽度=411μs (18位分辨率)
	Write_MAX30102_Reg(MAX30102_REG_SPO2_CONFIG,0x27);
	// LED脉冲幅度：红光=7.0mA，红外光=7.0mA(电流过高可能光线太强导致光电二极管饱和（信号达到最大值））
    Write_MAX30102_Reg(MAX30102_REG_LED1_PA, 0x24); 
    Write_MAX30102_Reg(MAX30102_REG_LED2_PA, 0x24);

	init_MAX30102_data();
}
void Reset_MAX30102(void){
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, 0x40);
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, 0x40);
}

void init_MAX30102_data(void){
	uint32_t red, ir;
	un_min=0x3FFFF;
	un_max=0;
	for(buffer_index=0;buffer_index<BUFFER_SIZE;buffer_index++){
		while (MAX30102_INT==1);//等待，直到中断引脚断言
		Read_MAX30102_FIFO(&red, &ir);
		red_buffer[buffer_index]=red;
		ir_buffer[buffer_index]=ir;
		if(un_min>red_buffer[buffer_index])
			un_min=red_buffer[buffer_index];    //更新计算最小值
		if(un_max<red_buffer[buffer_index])
			un_max=red_buffer[buffer_index];    //更新计算最大值
	}
	un_prev_data=red_buffer[buffer_index];
	//计算前500个样本（前5秒的样本）后的心率和血氧饱和度
	maxim_heart_rate_and_oxygen_saturation(ir_buffer, BUFFER_SIZE, red_buffer, &spo2, &spo2_valid, &heart_rate, &heart_rate_valid); 
}



/*
读取FIFO中的红外和红光数据
*/
void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir){
    uint8_t data[6]; 
	//清除状态寄存器
	Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1);
	Read_MAX30102_Reg(MAX30102_REG_INT_STATUS2);
    // 读取FIFO数据（一次读取6字节：红光3字节 + 红外3字节）
    Read_MAX30102_Reg_BYTES(MAX30102_REG_FIFO_DATA, data, 6); 
    // 组合18位数据
    *red = ((data[0] << 16) | (data[1] << 8) | data[2]) >> 6;
    *ir  = ((data[3] << 16) | (data[4] << 8) | data[5]) >> 6;    
    // 确保只保留18位有效数据
    *red &= 0x03FFFF;
    *ir  &= 0x03FFFF;
	printf("red=%lu   ir=%lu\n",*red,*ir);
}

void get_SpO2_HR(void){
	uint32_t red, ir;
	int32_t n_brightness;
	u8 dis_hr=0,dis_spo2=0;
	float f_temp;
	//舍去前100组样本，并将后400组样本移到顶部，将100~500缓存数据移位到0~400
	for(buffer_index=100;buffer_index<BUFFER_SIZE;buffer_index++){
		red_buffer[buffer_index-100]=red_buffer[buffer_index];
		ir_buffer[buffer_index-100]=ir_buffer[buffer_index];
		if(un_min>red_buffer[buffer_index])	//寻找移位后0-400中的最小值
			un_min=red_buffer[buffer_index];
		if(un_max<red_buffer[buffer_index])	//寻找移位后0-400中的最大值
			un_max=red_buffer[buffer_index];
	}
	//在计算心率前取100组样本，取的数据放在400-500缓存数组中
	for(buffer_index=400;buffer_index<BUFFER_SIZE;buffer_index++){
		while (MAX30102_INT==1);//等待，直到中断引脚断言
		Read_MAX30102_FIFO(&red, &ir);
		red_buffer[buffer_index]=red;
		ir_buffer[buffer_index]=ir;
		if(red_buffer[buffer_index]>un_prev_data){
			f_temp=red_buffer[buffer_index]-un_prev_data;
			f_temp/=(un_max-un_min);
			f_temp*=255;			//公式（心率曲线）=（新数值-旧数值）/（最大值-最小值）*255
			n_brightness-=(int)f_temp;
			if(n_brightness<0)
				n_brightness=0;
		}else{
			f_temp=un_prev_data-red_buffer[buffer_index];
			f_temp/=(un_max-un_min);
			f_temp*=255;			//公式（心率曲线）=（旧数值-新数值）/（最大值-最小值）*255
			n_brightness+=(int)f_temp;
			if(n_brightness>255)
				n_brightness=255;
		}
		if(heart_rate_valid==1&&spo2_valid==1&&heart_rate<120&&spo2<101){
			dis_hr = heart_rate;
			dis_spo2 = spo2;
		}else{
			dis_hr = 0;
			dis_spo2 = 0;
		}
	}
	maxim_heart_rate_and_oxygen_saturation(ir_buffer, BUFFER_SIZE, red_buffer, &spo2, &spo2_valid, &heart_rate, &heart_rate_valid); 
	printf("HR= %lu BPM SpO2= %lu\r\n ",dis_hr,dis_spo2);
}


/*
读取一组数据
*/
void Read_MAX30102_Reg_BYTES(uint8_t reg,uint8_t * data,uint8_t len){
	i2c_read_bytes(MAX30102_I2C_ADDR,reg, data, len);
}

void Start_MAX30102_Sampling(void){
	uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode & ~0x80);
}

void Stop_MAX30102_Sampling(void){
    uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode | 0x80);

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


/******************语音模块***********************/
void update_SpO2(void){
    static MAX30102 last_max30102 = SHUT_DOWN;
    if (last_max30102 != current_max30102) {
        reset_data();
        last_max30102 = current_max30102;
    }
    switch (current_max30102) {
        case SpO2:
            //get_real_SpO2();
			if(spo2>0){
				display_sensor((double)spo2);
			}	
            break;
        case HEART_RATE:
            //暂时未实现
            break;
        case SHUT_DOWN:
            break;
    }

}

void reset_data(void){
	buffer_index=0;
	heart_rate=0;
	heart_rate_valid=0;
	spo2=0;
	spo2_valid=0;
	memset(red_buffer,0,BUFFER_SIZE);
	memset(ir_buffer,0,BUFFER_SIZE);
}
void set_max30102_mode(MAX30102 mode){
	current_max30102=mode;
	if(current_max30102==SHUT_DOWN){
		reset_data();
	}

}

void get_SpO2(void){
	init_MAX30102();
	Start_MAX30102_Sampling();
	set_max30102_mode(SpO2);
}
void shut_down(void){
	set_max30102_mode(SHUT_DOWN);
	Stop_MAX30102_Sampling();
	display_close();
}


