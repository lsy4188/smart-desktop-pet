#include "systick.h"
#include "usart.h"
#include "SHT20.h"

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
	init_systick(168);
	init_usart();
	init_SHT20V1();
	printf("test starting!!!");
	while(1){
		Process_DataV1("hold");
		systick_ms(1000);
	}
}

