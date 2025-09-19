#include "usart.h"
#include "M74HC595.h"
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
	init_usart();
	init_M74HC595();
	printf("test starting!!!");
	
	while(1){
	display_data(0,0);		
		display_data(1,1);
		display_data(2,2);
	}

}

