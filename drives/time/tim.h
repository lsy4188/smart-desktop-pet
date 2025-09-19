#ifndef __TIM_H__
#define __TIM_H__
#include "stm32f4xx.h"
void init_tim6_basic(void);
void init_tim7_basic(void);
void init_tim2_common(void);
void init_tim3_common(void);
void init_tim4_common(void);
void delay_ms(uint16_t ms);
void delay_us(uint16_t us);


#endif

