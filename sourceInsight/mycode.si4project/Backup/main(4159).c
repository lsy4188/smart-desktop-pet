#include "usart.h"
#include "SHT20.h"
#include "M74HC595.h"
#include "systick.h"
#include "MAX30102.h"
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
	sht20 st;
	int i=0;
	uint8_t display_buffer[4] = {0};
	init_systick(168);
	init_usart();
	init_SHT20();
	init_M74HC595();
	init_MAX30102();
	printf("test starting!!!");
	while(1){
	st=Process_Data("hold");
	printf("tem=%.2f\trh=%.2f\n",st.tem,st.rh);
	systick_ms(1000);
	convertToSegCodes(st.tem,display_buffer);
	printf("buffer0=0x%02X\n",display_buffer[0]);
	printf("buffer1=0x%02X\n",display_buffer[1]);
	printf("buffer2=0x%02X\n",display_buffer[2]);
	printf("buffer3=0x%02X\n",display_buffer[3]);
	}

}

