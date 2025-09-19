#include "usart.h"
#include "SHT20.h"
#include "M74HC595.h"
#include "systick.h"
#include "MAX30102.h"
#include "tim.h"
#include "dma.h"
#include "adc.h"
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
//sht20数据就绪标记位
u8 sht20_flg=0;
uint32_t src[20]={	0x11112222,0x33334444,0x55556666,0x77778888,0x99990000,
					0x11112222,0x33334444,0x55556666,0x77778888,0x99990000,
					0x11112222,0x33334444,0x55556666,0x77778888,0x99990000,
					0x11112222,0x33334444,0x55556666,0x77778888,0x99990000,
				 };
uint32_t tar[20]={0};

void compare(void){
int i=0;
for(i=0;i<20;i++){
if(src[i]!=tar[i]){
	printf("no\r\n");
}else{
	printf("0x%x\r\n",tar[i]);
}
}
}
int main(void)
{
	sht20 st;
	uint8_t display_buffer[5] = {0};
	init_systick(168);
	init_tim_basic();
	init_usart();
	init_MAX30102();
	//init_ADC1();
	
	/*init_SHT20();
	init_M74HC595();*/
	
	printf("test starting!!!");
	while(1){
		systick_ms(1000);
		get_Data();
		/*printf("get_avg_data=%d\r\n",get_avg_data());*/
		/*if(sht20_flg){
			sht20_flg=0;
			st=Process_Data("hold");
			printf("tem=%.2f\trh=%.2f\n",st.tem,st.rh);
			convertToSegCodes(st.tem,display_buffer);
		}
		//display_sensor(st.tem);
	display_data(0,display_buffer[0]);
	display_data(1,display_buffer[1]);
	display_data(1,display_buffer[2]);
	display_data(2,display_buffer[3]);
	display_data(3,display_buffer[4]);	*/
	
	}
	/*init_usart();
	printf("test starting");
	init_DMA2(src, tar, 20);
	while(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)==RESET);
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
	printf("test ending");
	compare();*/

}

