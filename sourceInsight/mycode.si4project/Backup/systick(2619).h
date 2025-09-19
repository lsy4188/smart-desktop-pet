#ifndef __SYSTICK_H__
#define __SYSTICK_H__
#include <stm32f4xx.h>

void init_systick(u16 systick);

void systick_us(u16 us);

void systick_ms(u16 ms);

#endif

