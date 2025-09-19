#ifndef __SHT20_H__
#define __SHT20_H__
#include <stm32f4xx.h>
#include <string.h>
#include "usart.h"
#include "i2c.h"
// SHT20 I2C地址
#define SHT20_I2C_ADDR         		0x40
#define SHT20_T_HOLD_MASTER    		0xE3
#define SHT20_HR_HOLD_MASTER		0xE5
#define SHT20_T_NHOLD_MASTER     	0xF3
#define SHT20_HR_NHOLD_MASTER		0xF5

typedef struct{
	uint8_t flg;
	double tem;
	double rh;
}sht20;

void init_SHT20(void);
void Reset_SHT20(void);
uint16_t read_data(uint8_t cmd);
void Process_Data(char *str);



void init_SHT20V1(void);
void Reset_SHT20V1(void);
uint16_t read_dataV1(uint16_t cmd);
void Process_DataV1(char *str);
#endif

