#include "M74HC595.h"
#include <ctype.h>
#include <string.h>
//段选
const uint8_t seg[]={
	0x3F, // "0"
	0x06, // "1"
	0x5B, // "2"
	0x4F, // "3"
	0x66, // "4"
	0x6D, // "5"
	0x7D, // "6"
	0x07, // "7"
	0x7F, // "8"
	0x6F, // "9"
	0x77, // "A"
	0x7C, // "B"
	0x39, // "C"
	0x5E, // "D"
	0x79, // "E"
	0x71, // "F"
	0x80, //"."
};
//位选
const uint8_t pos[]={
	0x01, 
	0x02, 
	0x04, 
	0x08, 
};

void init_M74HC595(void){
	init_spi();
}

void display_data(uint8_t which,uint8_t data,uint8_t flg){
	which=pos[which];
	if(flg){
	data=seg[data];
	}
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//拉低NSS开始通讯
	send_data(which);
	send_data(data);
	systick_ms(2);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高NSS结束通讯
}

extern uint8_t display_buffer[4];
void convertToSegCodes(double num){
	int i=0;
	uint8_t segCode;
	int digit;
	char str[10]={0};
	memset(display_buffer,0,sizeof(display_buffer));
	sprintf(str,"%.2f",num);
	for (i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            digit = str[i] - '0';
            segCode = seg[digit];
            if (str[i+1] == '.') {
                segCode |= 0x80; // 设置DP位
            }
            printf("char->'%c' seg: 0x%02X\n", str[i], segCode);
			display_buffer[i]=segCode;
		}
    }
}


