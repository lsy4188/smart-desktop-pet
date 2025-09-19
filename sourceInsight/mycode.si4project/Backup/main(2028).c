#include "systick.h"
#include "usart.h"
#include "SHT20.h"
#include "MAX30102.h"
#include "tim.h"
#include "fan.h"

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
	//sht20 temp;
	init_systick(168);
	init_usart();
	init_fan();
	init_tim_common();
	printf("test starting!!!");
	while(1)
		{
			TIM_SetCompare1(TIM3,3000);
			systick_ms(2);
		}

}

