#ifndef __MAX30102_H__
#define __MAX30102_H__
#include <stm32f4xx.h>
#include "i2c.h"
#include "usart.h"
#include <string.h>

// MAX30102 I2C地址(7bit地址+1bit方向)
#define MAX30102_I2C_ADDR         0x57<<1
//MAX30102寄存器地址
#define MAX30102_REG_INT_STATUS1  0x00
#define MAX30102_REG_INT_STATUS2  0x01 
#define MAX30102_REG_INT_ENABLE   0x02
#define MAX30102_REG_FIFO_WR_PTR  0x04
#define MAX30102_REG_OVF_COUNTER  0x05
#define MAX30102_REG_FIFO_RD_PTR  0x06
#define MAX30102_REG_FIFO_DATA    0x07
#define MAX30102_REG_FIFO_CONFIG  0x08
#define MAX30102_REG_MODE_CONFIG  0x09
#define MAX30102_REG_SPO2_CONFIG  0x0A
#define MAX30102_REG_LED1_PA      0x0C
#define MAX30102_REG_LED2_PA      0x0D
#define MAX30102_REG_PILOT_PA     0x10
#define MAX30102_REG_MULTI_LED    0x11
#define MAX30102_REG_TEMP_INTG    0x1F
#define MAX30102_REG_TEMP_FRAC    0x20
#define MAX30102_REG_TEMP_INT     0x1F
#define MAX30102_REG_TEMP_FRA     0x20
#define MAX30102_REG_TEMP_CONFIG  0x21
#define MAX30102_REG_REV_ID       0xFE
#define MAX30102_REG_PART_ID      0xFF

typedef struct {
    uint32_t red_buffer[100];
    uint32_t ir_buffer[100];
    int index;
    float dc_red;
    float dc_ir;
    float ac_red;
    float ac_ir;
}SignalProcess;

void init_MAX30102(void);
uint8_t Read_MAX30102_Reg(uint8_t reg);
void Write_MAX30102_Reg(uint8_t reg, uint8_t value);
void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir);
float Read_MAX30102_Temperature(void);
uint8_t Get_MAX30102_PartID(void);
uint8_t Get_MAX30102_RevID(void);
void Reset_MAX30102(void);
void Start_MAX30102_Sampling(void);
void Stop_MAX30102_Sampling(void);
uint8_t Get_FIFO_SampleCount(void);
void init_SignalProcess(SignalProcess *sp);
void Process_Signal(SignalProcess *sp, uint32_t red, uint32_t ir);
void Calculate_HR_SpO2(SignalProcess *sp, float *hr, float *spo2);


#endif

