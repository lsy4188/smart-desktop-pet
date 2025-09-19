#include "usart.h"
#include "i2c.h"
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
	init_systick(168);
	init_usart();
	init_MAX30102();
	printf("test starting!!!");
	
	while(1){
		get_Data();
		
	
	}
}

