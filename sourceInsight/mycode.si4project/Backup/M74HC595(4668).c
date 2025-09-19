#include "M74HC595.h"

void init_M74HC595(void){
	init_spi();
}

void display_data(uint8_t pos,uint8_t seg){
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//拉低NSS开始通讯
	send_data(poss[pos]);
	send_data(segs[seg]);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高NSS结束通讯
}

