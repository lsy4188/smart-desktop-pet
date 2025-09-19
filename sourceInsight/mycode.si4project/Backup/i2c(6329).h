#ifndef __I2C_H__
#define __I2C_H__
#include <stm32f4xx.h>
#include "systick.h"
#define I2C1_SCL GPIO_Pin_6
#define I2C1_SDA GPIO_Pin_7
#define I2C1_CLOCK_SPEED 100000  // 100kHz

#define ACK 0
#define NACK 1
#define SCL_H GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define SCL_L GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define SDA_H GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SDA_L GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define SDA_READ GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)


void init_i2c(void);
void i2c_write(uint8_t address,uint8_t reg,uint8_t data);
uint8_t i2c_read(uint8_t address,uint8_t reg);
void i2c_read_bytes(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

/***************************************************************/
void init_i2c_cust(void);
void init_i2c_in(void);
void init_i2c_out(void);
void start_i2c_cust(void);
void stop_i2c_cust(void);
void send_ACK_cust(void);
void send_NACK_cust(void);
uint8_t wait_ACK_cust(void);
uint8_t read_data_cust(uint8_t flg);
void write_data_cust(uint8_t data);



#endif

