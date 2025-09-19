#include "usart.h"
#include "wifi.h"
#include "sht20.h"
#include "tim.h"
#include "ASRPRO.h"
#include "M74HC595.h"
#include "sg90.h"

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
//sht20数据
uint8_t sensor[5];
uint8_t st_index=0;

#ifdef 0


extern sht20 sh;
int main(void)
{
	
	init_usart6();
	init_tim2_common();//温湿度
	init_tim6_basic();//延时
	init_tim4_common();//数码管显示(有问题)
	//printf("TIM4 initialized\n");  // 测试TIM4初始化是否会引发问题
	init_SHT20();
	init_M74HC595();
	printf("test starting!!!");
	while(1){
		if(sh.flg){
			sh.flg=0;
			Process_Data("hold");
			display_sensor(sh.tem);
			//printf("tem=%.2f\trh=%.2f\n",sh.tem,sh.rh);
			//convertToSegCodes(st.tem,display_buffer);
		}	
	}
}


extern uint32_t red_buffer[BUFFER_SIZE];
extern uint32_t ir_buffer[BUFFER_SIZE];
extern int buffer_index;
int main(void)
{
	uint32_t red=0, ir=0;
	init_tim6_basic();
	init_usart3();
	init_MAX30102();
	printf("test starting!!!");
	Start_MAX30102_Sampling();
	while(1){
		 	// 读取FIFO数据
            Read_MAX30102_FIFO(&red, &ir);
			/*delay_ms(200);
			//printf("red=%d   ir=%d\n",red,ir);
            // 存储到缓冲区
            red_buffer[buffer_index] = red;
            ir_buffer[buffer_index] = ir;
            // 处理健康数据
            process_health_data();*/
	}
}


int main(void)
{
	init_usart6();
	init_sg90(1);
	printf("start testing sg90.......\r\n");
	while(1){
		test();
	}
}

extern wifi_buffer wb;
int main(void)
{	
	init_wifi_mode();
	while(1){
		wifi_connect_process();
		
	}
}

extern ASRPRO asr;
int main(void)
{
	init_ASRPRO();
	while(1){
		deal_with_asr();
		update_display();
		update_action();
		update_wifi_mode();
	}
}
int main(void)
{
	init_usart6();
	init_sg90(1);
	printf("start testing sg90.......\r\n");
	while(1){
		get_down();
	}
}

int main(void)
{
	init_ASRPRO();
	while(1){
		deal_with_asr();
		update_display();
		update_action();
		update_wifi_mode();
	}
}

#endif

int main(void)
{
	init_usart6();
	init_sg90(0);
	printf("start testing sg90.......\r\n");
	while(1){
		turn_left() ;
	}
}











