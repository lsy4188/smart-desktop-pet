#include "systick.h"
#include "usart.h"
#include "tim.h"
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
	init_usart();
	init_systick(168);
	init_tim_common();
	while(1){ 
		if(dir==0){
			cvalue++;
		}else{
			cvalue--;
		}
		if(cvalue>=2500){
			dir=1;
		}
		if(cvalue==0){
			dir=0;
		}
		TIM_SetCompare1(TIM13,cvalue);
		systick_ms(2);
	}
}

