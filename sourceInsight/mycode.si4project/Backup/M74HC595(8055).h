#ifndef __M74HC595_H__
#define __M74HC595_H__
#include <stm32f4xx.h>
#include "spi.h"
const uint8_t segs[]={
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
const uint8_t poss[]={
	0x01, 
	0x02, 
	0x04, 
	0x08, 
};
void init_M74HC595(void);
/**************************************
display_data
函数功能：pos-位选；seg-段选
****************************************/
void display_data(uint8_t pos,uint8_t seg);
#endif
