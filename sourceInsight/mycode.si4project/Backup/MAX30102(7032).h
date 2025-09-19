#ifndef __MAX30102_H__
#define __MAX30102_H__
#include <stm32f4xx.h>
#include "i2c.h"
void init_MAX30102(void);
uint8_t Read_MAX30102_Reg(uint8_t reg);
void Write_MAX30102_Reg(uint8_t reg, uint8_t value);
void Read_MAX30102_FIFO(uint32_t *red, uint32_t *ir);
uint8_t Get_MAX30102_PartID(void);
uint8_t Get_MAX30102_RevID(void);
void Reset_MAX30102(void);
void Start_MAX30102_Sampling(void);
void Stop_MAX30102_Sampling(void);
#endif
