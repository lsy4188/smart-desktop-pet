#include "MAX30102.h"

uint32_t red_buffer[BUFFER_SIZE];
uint32_t ir_buffer[BUFFER_SIZE];
int buffer_index = 0;

void init_MAX30102(void){
	//初始化i2c
	init_i2c();
	//FIFO指针清零
	Write_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
	Write_MAX30102_Reg(MAX30102_REG_OVF_COUNTER, 0x00);
	Write_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
	//配置FIFO：采样平均=4，回滚使能，FIFO满阈=17
	Write_MAX30102_Reg(MAX30102_REG_FIFO_CONFIG,0x5F);
	// 模式配置：SpO2模式
	Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG,0x03);
	// SPO2配置：设置 SpO2 传感器中 ADC 的满量程范围4096,采样率100Hz，LED脉冲宽度=411μs (18位分辨率)
	Write_MAX30102_Reg(MAX30102_REG_SPO2_CONFIG,0x27);
	// LED脉冲幅度：红光=24.8mA，红外光=24.8mA
    Write_MAX30102_Reg(MAX30102_REG_LED1_PA, 0x7C); 
    Write_MAX30102_Reg(MAX30102_REG_LED2_PA, 0x7C); 
    // 多LED控制：启用LED1红光和LED2红外光(在SpO2模式下内部已配好)
   // Write_MAX30102_Reg(MAX30102_REG_MULTI_LED1, 0x21);
	 // 使能新数据就绪中断和温度就绪中断
    Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE1, 0x40);
	Write_MAX30102_Reg(MAX30102_REG_INT_ENABLE2, 0x02);
    // 温度测量配置
    Write_MAX30102_Reg(MAX30102_REG_TEMP_CONFIG, 0x01);
}

void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir){
	uint8_t wr_ptr,rd_ptr,num_samples;
	uint8_t data[6];
	wr_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
	//注：在一个事务中连续读取多个字节，这样读指针才会自动递增
    rd_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
    num_samples = (wr_ptr - rd_ptr) & 0x1F; // 32样本环形缓冲 
    printf("num_samples===>%d\r\n",num_samples);
    if(num_samples>0){
		Read_MAX30102_RegV1(MAX30102_REG_FIFO_DATA, data, 6);
		 // 组合数据 (18位有效数据)
		*red = ((data[0] << 16) | (data[1] << 8) | data[2]) >> 6;
		*ir = ((data[3] << 16) | (data[4] << 8) | data[5]) >> 6;

	}else{
		*red = 0;
		*ir = 0;

	}

}

void Read_MAX30102_FIFOV1(uint32_t *red, uint32_t *ir){
	uint8_t wr_ptr, rd_ptr, ovf,fifo_level;
	uint8_t data[6];
	int i=0;
	printf("*********************************************\r\n"); 
	wr_ptr=Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
	printf("read before wr_ptr: 0x%x\r\n", wr_ptr);
	rd_ptr=Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
	printf("read before rd_ptr: 0x%x\r\n", rd_ptr);
	ovf=Read_MAX30102_Reg(MAX30102_REG_OVF_COUNTER);
	printf("ovf: 0x%x\r\n", ovf);
	fifo_level=(wr_ptr-rd_ptr)%32;
	printf("FIFO Level before: %d\r\n", fifo_level); 
	if(ovf==0x1F){
		printf("FIFO over! lost %d data\n", ovf);
		Write_MAX30102_Reg(MAX30102_REG_OVF_COUNTER, 0x00);
	    Write_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
        Write_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
        *red = 0;
        *ir = 0;
        return;
	}
	while(fifo_level>0){
		for (i=0; i<6; i++) {
            data[i] = Read_MAX30102_Reg(0x07); // 读取FIFO数据
            rd_ptr = Read_MAX30102_Reg(0x06);
			printf("for %d is rd_ptr: 0x%x\r\n",i, rd_ptr);
        }

        *red = ((data[0] << 16) | (data[1] << 8) | data[2]) >> 6;
        *ir  = ((data[3] << 16) | (data[4] << 8) | data[5]) >> 6;
        // 更新指针
        wr_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
		printf("read after wr_ptr: 0x%x\r\n", wr_ptr);
        rd_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
		printf("read after rd_ptr: 0x%x\r\n", rd_ptr);
        fifo_level = (wr_ptr - rd_ptr) % 32;
		printf("FIFO Level after: %d\r\n", fifo_level);
	}
	printf("*********************************************\r\n"); 
}


void Read_MAX30102_FIFOV2(uint32_t *red, uint32_t *ir){
	uint8_t wr_ptr,rd_ptr,num_samples,ovf;
	uint8_t data[6]={0};
	wr_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR);
	//注：在一个事务中连续读取多个字节，这样读指针才会自动递增
    rd_ptr = Read_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR);
    num_samples = (wr_ptr - rd_ptr) & 0x1F; // 32样本环形缓冲 
	ovf=Read_MAX30102_Reg(MAX30102_REG_OVF_COUNTER);
	if(ovf==0x1F){
		printf("FIFO OVERF LOW!........");
		Write_MAX30102_Reg(MAX30102_REG_OVF_COUNTER, 0x00);
		Write_MAX30102_Reg(MAX30102_REG_FIFO_WR_PTR, 0x00);
		Write_MAX30102_Reg(MAX30102_REG_FIFO_RD_PTR, 0x00);
		*red = 0;
		*ir = 0;
		return;
	}
    if (num_samples > 0) {  
        Read_MAX30102_RegV1(MAX30102_REG_FIFO_DATA, data, 6);
        *red = ((data[0] << 16) | (data[1] << 8) | data[2]) >> 6;
        *ir  = ((data[3] << 16) | (data[4] << 8) | data[5]) >> 6;
    }
}

void Read_MAX30102_RegV1(uint8_t reg,uint8_t * data,uint8_t len){
	i2c_read_bytes(MAX30102_I2C_ADDR,reg, data, len);
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
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode & ~0x80);
}

void Stop_MAX30102_Sampling(void){
    uint8_t mode = Read_MAX30102_Reg(MAX30102_REG_MODE_CONFIG);
    Write_MAX30102_Reg(MAX30102_REG_MODE_CONFIG, mode | 0x80); // 设置关机位

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
		//Read_MAX30102_FIFOV2(&red,&ir);
	}
	tem=Read_MAX30102_Temperature();
	printf("red=%d   ir=%d   tem=%.2f\n",red,ir,tem);
}
/**********************************************************/

void preprocess_signals(float* red_filtered, float* ir_filtered) {
    static float red_x[3] = {0}, red_y[3] = {0};
    static float ir_x[3] = {0}, ir_y[3] = {0};
    static float red_ma_buffer[10] = {0}, ir_ma_buffer[10] = {0};
    static int ma_index = 0;
    // 获取当前样本
    uint32_t red_sample = red_buffer[buffer_index];
    uint32_t ir_sample = ir_buffer[buffer_index];
    // 1. 带通滤波去除直流分量和噪声
    float red_bp = bandpass_filter(red_sample, red_x, red_y);
    float ir_bp = bandpass_filter(ir_sample, ir_x, ir_y);
    // 2. 移动平均平滑
    *red_filtered = moving_average(red_bp, red_ma_buffer, &ma_index, 10);
    *ir_filtered = moving_average(ir_bp, ir_ma_buffer, &ma_index, 10);
    // 更新缓冲区索引
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
}

// 带通滤波器实现 (0.5-5Hz, 对应30-300BPM)
float bandpass_filter(float input, float* x, float* y) {
    // 滤波器系数 (针对100Hz采样率)
    static const float b[3] = {0.0084, 0, -0.0084};
    static const float a[3] = {1, -1.737, 0.983};
    // 更新输入历史
    x[2] = x[1];
    x[1] = x[0];
    x[0] = input;
    // 计算输出
    y[2] = y[1];
    y[1] = y[0];
    y[0] = b[0]*x[0] + b[1]*x[1] + b[2]*x[2] - a[1]*y[1] - a[2]*y[2];
    return y[0];
}
// 移动平均滤波器
float moving_average(float input, float* buffer, int* index, int size) {
    static float sum = 0;
    // 减去最旧的值
    sum -= buffer[*index];
    // 添加新值
    buffer[*index] = input;
    sum += input;
    // 更新索引
    *index = (*index + 1) % size;
    return sum / size;
}

float calculate_heart_rate(float* red_signal) {
    static float last_peak = 0;
    static float last_valley = 0;
    static float heart_rates[5] = {0};
    static int hr_index = 0;
    // 寻找峰值 (简单阈值法)
    static int peak_count = 0;
    static float max_val = -10000;
    static int max_index = 0;
    if (*red_signal > max_val) {
        max_val = *red_signal;
        max_index = buffer_index;
    }
    // 检测峰值
    if (*red_signal < 0.8 * max_val && max_val > 0.5) {
        float current_peak = max_val;
        int current_index = max_index;
        // 计算心率 (BPM)
        float interval = (current_index - last_peak) / 100.0; // 100Hz采样率
        float hr = 60.0 / interval;
		// 取中值滤波
        float sorted[5];
		float temp;
		int i,j;
        // 心率范围检查
        if (hr > MIN_HEART_RATE && hr < MAX_HEART_RATE) {
            heart_rates[hr_index] = hr;
            hr_index = (hr_index + 1) % 5;
            memcpy(sorted, heart_rates, sizeof(sorted));
            // 排序
            for (i = 0; i < 4; i++) {
                for (j = i+1; j < 5; j++) {
                    if (sorted[j] < sorted[i]) {
                        temp = sorted[i];
                        sorted[i] = sorted[j];
                        sorted[j] = temp;
                    }
                }
            }
            last_peak = current_index;
            max_val = -10000;
            peak_count++;
            // 有足够峰值后返回稳定心率
            if (peak_count > 5) {
                return sorted[2]; // 返回中值
            }
        }
    }
    return 0; // 尚未检测到有效心率
}

float calculate_spo2(float red_signal, float ir_signal) {
    static float red_ac_sum = 0, red_dc_sum = 0;
    static float ir_ac_sum = 0, ir_dc_sum = 0;
    static int sample_count = 0;
    // 计算AC和DC分量
    red_ac_sum += fabs(red_signal);
    red_dc_sum += red_buffer[buffer_index]; 
    ir_ac_sum += fabs(ir_signal);
    ir_dc_sum += ir_buffer[buffer_index];
    sample_count++;
    // 每1秒计算一次 (100个样本)
    if (sample_count >= 100) {
        float red_ac = red_ac_sum / sample_count;
        float red_dc = red_dc_sum / sample_count;
        float ir_ac = ir_ac_sum / sample_count;
        float ir_dc = ir_dc_sum / sample_count;
        // 计算R值
        float R = (red_ac / red_dc) / (ir_ac / ir_dc);
        // 使用经验公式计算SpO2
        float spo2 = CALIBRATION_CONST_A - CALIBRATION_CONST_B * R;
        // 复位累加器
        red_ac_sum = 0;
        red_dc_sum = 0;
        ir_ac_sum = 0;
        ir_dc_sum = 0;
        sample_count = 0;
        // 范围限制 (正常SpO2在70%-100%)
        if (spo2 < 70) spo2 = 70;
        if (spo2 > 100) spo2 = 100;
        return spo2;
    }
    return -1; // 数据不足
}

void process_health_data(void) {
    float red_filtered, ir_filtered,heart_rate,spo2;
	 static int temp_counter = 0;
    // 1. 信号预处理
    preprocess_signals(&red_filtered, &ir_filtered);
    // 2. 计算心率
    heart_rate = calculate_heart_rate(&red_filtered);
    // 3. 计算血氧饱和度
    spo2 = calculate_spo2(red_filtered, ir_filtered);
    // 4. 输出结果
    if (heart_rate > 0) {
        printf("Heart Rate: %.1f BPM\n", heart_rate);
    }
    if (spo2 > 0) {
        printf("SpO2: %.1f%%\n", spo2);
    }
	// 5. 温度读取 (可选)
    if (temp_counter++ >= 100) { // 每100个样本读一次温度
        float temperature = Read_MAX30102_Temperature();
        printf("Temperature: %.2f C\n", temperature);
        temp_counter = 0;
    }
}


