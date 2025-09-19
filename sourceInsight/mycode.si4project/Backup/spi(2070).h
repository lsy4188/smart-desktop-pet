#ifndef __SPI_H__
#define __SPI_H__
#include <stm32f4xx.h>
void init_spi(void);
void send_data(uint8_t data);
#endif
