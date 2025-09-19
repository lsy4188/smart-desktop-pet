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

void display_data(uint8_t which,uint8_t data){
	
	send_data(pos[which]);
	send_data(seg[data]);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//拉低NSS开始通讯
	delay_ms(1);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高NSS结束通讯
}

void convertToSegCodes(double num,uint8_t *display_buffer){
	uint8_t i=0;
	uint8_t digit;
	char str[10]={0};
	sprintf(str,"%.2f",num);
	for (i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            digit = str[i] - '0';
		}else{
			digit=16;
		}
		display_buffer[i]=digit;
    }
}

void display_sensor(double num){
		uint8_t sensor[5];
		int i=0,j=0;
		int digit;
		char str[10]={0};
		memset(sensor,0,sizeof(sensor));
		sprintf(str,"%.2f",num);
		for (i = 0; str[i] != '\0'; i++) {
			if (isdigit(str[i])) {
				digit = str[i] - '0';
			}else{
				digit=16;
			}
			sensor[i]=digit;
		}
		for(i=0,j=0;i<4;i++,j++){
			display_data(i,sensor[j]);	
			if(i!=3&&sensor[i+1]==16){
				display_data(i,sensor[++j]);
			}
		}
}


