#ifndef __KEY_H__
#define __KEY_H__
#include <stm32f4xx.h>
#include "stdio.h"
#include "string.h"
typedef enum{
	LED			=1,
	BEEP		=2,
	FAN			=3
} Mode;

/**************************************
init_key
函数功能：初始化
****************************************/
void init_key(void);
/**************************************
get_key_val_diff
函数功能：不同一按键多功能实现
输出参数：按键是否按下
****************************************/ 
u8 get_key_val_diff(void);

/**************************************
get_key_val_same
函数功能：单一按键多功能实现
输入参数：num:控制的设备数量
输出参数：按键是否按下
****************************************/ 
u8 get_key_val_same(u8 num);

/**************************************
init_key_INT
函数功能：初始化(中断)
****************************************/ 
void init_key_INT(void);


#endif

