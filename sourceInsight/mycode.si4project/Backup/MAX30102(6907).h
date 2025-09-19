#ifndef __MAX30102_H__
#define __MAX30102_H__
#include <stm32f4xx.h>
#include "i2c.h"
#include "usart.h"
#include <string.h>
#include <math.h>
#include "M74Hc595.h"
#include "algorithm.h"
#include "FreeRTOS.h"
#include "task.h"


// MAX30102 I2C地址
#define MAX30102_I2C_ADDR         	0x57
//MAX30102寄存器地址
#define MAX30102_REG_INT_STATUS1  	0x00
#define MAX30102_REG_INT_STATUS2  	0x01 
#define MAX30102_REG_INT_ENABLE1  	0x02
#define MAX30102_REG_INT_ENABLE2   	0x03
#define MAX30102_REG_FIFO_WR_PTR  	0x04
#define MAX30102_REG_OVF_COUNTER  	0x05
#define MAX30102_REG_FIFO_RD_PTR  	0x06
#define MAX30102_REG_FIFO_DATA    	0x07
#define MAX30102_REG_FIFO_CONFIG  	0x08
#define MAX30102_REG_MODE_CONFIG  	0x09
#define MAX30102_REG_SPO2_CONFIG  	0x0A
#define MAX30102_REG_LED1_PA      	0x0C
#define MAX30102_REG_LED2_PA     	0x0D
#define MAX30102_REG_PILOT_PA     	0x10
#define MAX30102_REG_MULTI_LED1    	0x11
#define MAX30102_REG_MULTI_LED2    	0x12
#define MAX30102_REG_TEMP_INTG    	0x1F
#define MAX30102_REG_TEMP_FRAC    	0x20
#define MAX30102_REG_TEMP_CONFIG  	0x21
#define MAX30102_REG_REV_ID       	0xFE
#define MAX30102_REG_PART_ID      	0xFF

//MAX30102中断引脚使用(PC2)
#define PCin(n)   (GPIOC->IDR & (1 << n)) ? 1 : 0


typedef enum{
	SpO2		=0,
	HEART_RATE	=1,
	SHUT_DOWN	=2
}MAX30102;
extern MAX30102 max30102;

void init_MAX30102(void);
uint8_t Read_MAX30102_Reg(uint8_t reg);
void Write_MAX30102_Reg(uint8_t reg, uint8_t value);
void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir);
float Read_MAX30102_Temperature(void);
uint16_t Get_MAX30102_samples(void);
uint8_t MAX30102_samples_data_is_ready(void);
void Start_MAX30102_Sampling(void);
void Stop_MAX30102_Sampling(void);
void Read_MAX30102_Reg_BYTES(uint8_t reg,uint8_t * data,uint8_t len);
void get_real_SpO2(void);
void get_real_SpO2_task(uint32_t timeout);


/**************语音模块********************/
void update_SpO2(void);

void reset_data(void);

void set_max30102_mode(MAX30102 mode);

void get_SpO2(void);

void shut_down(void);


#endif

