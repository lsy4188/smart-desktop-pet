#include "systick.h"
#include "usart.h"
#include "SHT20.h"
#include "MAX30102.h"
#include "tim.h"
#include "key.h"

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

u16 cvalue=50;
int main(void)
{
	//sht20 temp;
	init_systick(168);
	init_usart();
	init_key();
	init_tim_common();
	//init_SHT20V1();
	//init_MAX30102();
	
	printf("test starting!!!");
	while(1){
		if(cvalue>3000) cvalue=3000;
		if(cvalue<=0) cvalue=0;
		TIM_SetCompare1(TIM13,cvalue);
		//temp=Process_DataV1("hold");
		//printf("tem=%.2f\trh=%.2f\n",temp.tem,temp.rh);
		//get_Data();
		//systick_ms(1000);
		systick_us(5);
		printf("cvalue=%d\n",cvalue);
	}
}

