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

uint16_t led_pwm=0;
uint8_t dir=0;

int main(void)
{
	//sht20 temp;
	init_systick(168);
	init_usart();
	init_fan();
	init_tim_common();
	
	//init_SHT20V1();
	//init_MAX30102();
	
	printf("test starting!!!");
	while(1)
		{
			if(dir == 0)//方向，决定led_pwm值是变大还是变小
			{
				led_pwm++;
			}
			else
			{
				led_pwm--;
			}
			if(led_pwm>500)//最大值(最亮)，到了就改变方向
			{
				dir = 1;
			}
			if(led_pwm == 0)
			{
				dir = 0;
			}
			TIM_SetCompare1(TIM3,led_pwm);//设置比较值，改变PWM的占空比
			systick_ms(2);
		}

}

