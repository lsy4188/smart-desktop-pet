#include "usart.h"
#include "wifi.h"
#include "sht20.h"
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
//sht20数据
sht20 st;
uint8_t sensor[5];
uint8_t current_digit=0;
/*
wifi模组
*/
uint8_t usart3_index=0;
uint8_t usart3_isready=0;
uint8_t usart3_arr[256]={0};

int main(void)
{	
	init_wifi_mode();
	send_tcp_data("192.168.20.52", 8080, "start send MSG");
	while(1){
		receve_wifi_response("OK");
		delay_ms(10);
	}
}

