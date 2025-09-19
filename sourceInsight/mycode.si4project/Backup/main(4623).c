#include "systick.h"
#include "MAX30102.h"
#include "usart.h"
int main(void)
{
	uint32_t red_value, ir_value;
	uint8_t part_id, rev_id;
	init_usart();
	init_MAX30102();
	init_systick(168);
	// 检查器件ID
    part_id = Get_MAX30102_PartID();
    rev_id = Get_MAX30102_RevID();
    printf("Part ID: 0x%02X, Rev ID: 0x%02X\r\n", part_id, rev_id);
	Start_MAX30102_Sampling();
	while(1){ 	
		uint8_t status=Read_MAX30102_Reg(MAX30102_REG_INT_STATUS);
		if(status&0x80){
			Read_MAX30102_FIFO(&red_value,&ir_value);
			 // 输出数据
            printf("Red: %6lu, IR: %6lu\r\n", red_value, ir_value);
            // 清除中断标志
            Write_MAX30102_Reg(MAX30102_REG_INT_STATUS, status);
		}
		systick_ms(500);
	}
}

