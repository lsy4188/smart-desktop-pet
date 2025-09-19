#ifndef __DMA_H__
#define __DMA_H__
#include <stm32f4xx.h>
#define BufferSize 200
uint16_t buffer[BufferSize]={0};
void init_ADC1(void);
uint16_t get_avg_data(void);

#endif

