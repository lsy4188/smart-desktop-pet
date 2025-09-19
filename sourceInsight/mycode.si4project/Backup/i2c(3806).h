#ifndef __I2C_H__
#define __I2C_H__
#include <stm32f4xx.h>
void init_i2c(void);

void i2c_write(uint8_t address,uint8_t reg,uint8_t data);

uint8_t i2c_read(uint8_t address,uint8_t reg);




#endif
