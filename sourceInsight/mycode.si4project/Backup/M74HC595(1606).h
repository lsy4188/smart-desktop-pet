#ifndef __M74HC595_H__
#define __M74HC595_H__
#include <stm32f4xx.h>
#include "spi.h"
#include "tim.h"
#include "usart.h"

uint8_t sensor[5];
uint8_t flag=0;

void init_M74HC595(void);
/**************************************
display_data
函数功能：which-选择的数码管；data-显示的数据
****************************************/
void display_data(uint8_t which,uint8_t data);

void convertToSegCodes(double num,uint8_t *display_buffer);

void display_sensor(double num);





#endif

