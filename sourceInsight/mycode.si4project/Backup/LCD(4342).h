#ifndef __LCD_H__
#define __LCD_H__
#include <stm32f4xx.h>
#include "spi.h"
/*
CS-PB12
RST-PA8
D/C-PA9
SID-PB15
SCL-PB13
LED-PA10
SDO-PB14
*/
#define LCD_CS_SET GPIO_ResetBits(GPIOB, GPIO_Pin_12);
#define LCD_CS_CLEAR GPIO_SetBits(GPIOB, GPIO_Pin_12);

#define LCD_RESET GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define LCD_NO_RESET GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define LCD_DATA_SET GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define LCD_COMMAND_SET GPIO_ResetBits(GPIOA, GPIO_Pin_9)

void init_lcd(void);

#ifndef

