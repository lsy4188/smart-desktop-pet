#ifndef __KEY_H__
#define __KEY_H__
#include <stm32f4xx.h>
#include "string.h"
typedef enum{
	DEFALUT		=0,
	KEY1		=1,
	KEY2		=2,
	KEY3		=3
} KEY;

extern KEY key_which;

/**************************************
init_key
函数功能：初始化
****************************************/
void init_key(void);

/**************************************
init_key_INT
函数功能：初始化(中断)
****************************************/ 
void init_key_INT(void);

uint8_t detect_key(void);


#endif

