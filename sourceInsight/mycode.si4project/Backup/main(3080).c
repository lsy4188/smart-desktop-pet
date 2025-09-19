#include "systick.h"
#include "usart.h"
#include "tim.h"
#include "MAX30102.h"

#define LED_BIT  0
#define BEEP_BIT 1
#define FAN_BIT  2
u8 key_flag=0;
u8 mode=0;
/*
定义通讯规则
*/
#define CMD_BUFFER_SIZE 32
u8 cache[CMD_BUFFER_SIZE];
u16 cmd_ready=0;
u16 cmd_index=0;

u16 cvalue=0;
u8 dir=0;

int main(void)
{
    // 温度读取计时器
   uint32_t temp_timer = 0,current_time=0;
	uint8_t samples=0,status1=0,status2=0;
	uint32_t red, ir;
	int i=0;
	float temper,hr, spo2,signal_quality;
	static uint32_t last_calc = 0;
	SignalProcess sp;
	// 时间戳变量
    uint32_t last_temp_time = 0;
    uint32_t last_output_time = 0;
    uint32_t last_sample_time = 0;
	init_usart();
	init_systick(168);
	init_MAX30102();
	printf("MAX30102 Initialized. Part ID: 0x%02X\n", Get_MAX30102_PartID());
	
	init_SignalProcess(&sp);
	Start_MAX30102_Sampling();
	while(1){ 
		current_time = GetTick();
		status1=Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1);
		status2=Read_MAX30102_Reg(MAX30102_REG_INT_STATUS2);
		if(status1&0x40){
			Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1);
			samples=Get_FIFO_SampleCount();
		 for(i=0; i<samples; i++){
                Read_MAX30102_FIFO(&red, &ir);
                // 信号处理
                Process_Signal(&sp, red, ir);
                // 输出原始数据 (调试用)
                 printf("R:%6lu IR:%6lu\n", red, ir);
            }
		   last_sample_time = current_time;
		}
		// 处理温度就绪中断 (每10秒读取一次)
		if(status2&0x02||(current_time - last_temp_time) > 10000){
			Read_MAX30102_Reg(MAX30102_REG_INT_STATUS2);
			temper=Read_MAX30102_Temperature();
			printf("Temperature: %.2fC\n", temper);
		}
		// 每1秒计算并输出一次心率/血氧
        if((current_time - last_calc) > 1000){
            Calculate_HR_SpO2(&sp, &hr, &spo2);
            // 信号质量检测
           	signal_quality  = (sp.dc_red > 50000 && sp.dc_red < 200000) ? 1.0f : 0.0f;
            printf("HR: %.0f BPM | SpO2: %.0f%% | Quality: %.0f%%\n", 
                  hr, spo2, signal_quality*100);
            last_output_time = current_time;
        }
		
	}
}

