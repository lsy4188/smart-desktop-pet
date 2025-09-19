#include "systick.h"
#include "usart.h"
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
	// 时间戳变量
	uint32_t last_temp_time = 0;
	uint32_t last_output_time = 0;
	uint32_t last_sample_time = 0;
	uint32_t current_time = 0;
	uint8_t samples = 0, status1 = 0, status2 = 0;
	uint32_t red = 0, ir = 0;
	float temper = 0, hr = 0, spo2 = 0;
	int i =0;
	SignalProcess sp;
	
	init_usart();
    init_systick(168);
    init_MAX30102();
    printf("MAX30102 Initialized. Part ID: 0x%02X\n", Get_MAX30102_PartID());
    init_SignalProcess(&sp);
    Start_MAX30102_Sampling();
	while(1){ 
		current_time = GetTick();
        
        // 检查中断状态
        status1 = Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1);
        status2 = Read_MAX30102_Reg(MAX30102_REG_INT_STATUS2);
        
        // 处理环境光溢出中断
        if(status1 & 0x20) {  // ALC_OVF标志
            Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1); // 清除中断
            printf("Ambient light overflow! Adjust sensor position.\n");
        }
        
        // 处理FIFO数据就绪中断
        if(status1 & 0x40) {  // PPG_RDY标志
            Read_MAX30102_Reg(MAX30102_REG_INT_STATUS1); // 清除中断
            samples = Get_FIFO_SampleCount();
            
            for(i = 0; i < samples; i++) {
                Read_MAX30102_FIFO(&red, &ir);
                Process_Signal(&sp, red, ir);
                
                // 仅当有手指时输出原始数据
                if (sp.finger_detected) {
                    printf("R:%6lu IR:%6lu\n", red, ir);
                }
            }
            last_sample_time = current_time;
        }
        
        // 处理温度就绪中断（每30秒读取一次）
        if((status2 & 0x02) || (current_time - last_temp_time > 30000)) {
            if (status2 & 0x02) {
                Read_MAX30102_Reg(MAX30102_REG_INT_STATUS2); // 清除中断
            }
            temper = Read_MAX30102_Temperature();
            printf("Temperature: %.2fC\n", temper);
            last_temp_time = current_time;
        }
        
        // 每秒计算并输出一次心率/血氧
        if((current_time - last_output_time) > 1000) {
            if (sp.finger_detected) {
                Calculate_HR_SpO2(&sp, &hr, &spo2);
                printf("HR: %.0f BPM | SpO2: %.0f%%\n", hr, spo2);
            } else {
                printf("No finger detected!\n");
            }
            last_output_time = current_time;
        }
	}
}

