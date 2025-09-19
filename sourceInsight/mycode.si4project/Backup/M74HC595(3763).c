#include "M74HC595.h"
#include <ctype.h>
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
	which=pos[which];
	data=seg[data];
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//拉低NSS开始通讯
	send_data(which);
	send_data(data);
	systick_ms(2);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高NSS结束通讯
}

void convertToSegCodes(double num){
	int i=0;
	char str[10]={0};
	sprintf(str,"%.2f",num);
	for (i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            int digit = str[i] - '0'; // 字符转数字
            uint8_t segCode = seg[digit]; // 获取基本段选码
            // 检查下一个字符是否为小数点
            if (str[i+1] == '.') {
                segCode |= 0x80; // 设置DP位
            }
            printf("字符 '%c' 的段选码: 0x%02X\n", str[i], segCode);
        }
    }
}


