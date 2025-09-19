#include "systick.h"
#include "MAX30102.h"
#include "usart.h"
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
int main(void)
{
	float red_percent;
	float ir_percent;
	uint8_t part_id, rev_id;
	uint32_t sample_count = 0;
	int i=0;
	uint8_t int_en=0;
	init_usart();
	init_MAX30102();
	init_systick(168);
	// 检查器件ID
  	part_id = Get_MAX30102_PartID();
  	rev_id = Get_MAX30102_RevID();
  	printf("Part ID: 0x%02X, Rev ID: 0x%02X\r\n", part_id, rev_id);
	// 验证配置
    int_en = Read_MAX30102_Reg(MAX30102_REG_INT_ENABLE);
    printf("MAX30102_REG_INT_ENABLE: 0x%02X\n", int_en);
	
	Start_MAX30102_Sampling();
	printf("start samping...\r\n");

	while(1){ 	
		// 读取中断状态
        uint8_t status = Read_MAX30102_Reg(MAX30102_REG_INT_STATUS);
        // 处理FIFO满中断
        if (status & 0x80) {
            uint8_t samples = Get_FIFO_SampleCount();
            if (samples > 0) {
                sample_count += samples;
                printf("read %d samping (sum_spaming: %lu)\r\n", samples, sample_count);
                for (i = 0; i < samples; i++) {
                    uint32_t red, ir;
                    Read_MAX30102_FIFO(&red, &ir);
                    // 计算信号强度百分比
                    red_percent = (red / 262143.0) * 100; // 18位最大值262143
                    ir_percent = (ir / 262143.0) * 100;
                    // 这里可以添加心率/血氧算法
                    printf("spaing %d: R=%.1f%%, IR=%.1f%%\r\n", 
                           i, red_percent, ir_percent);
                }
            }
        }
		systick_ms(500);
	}
}

