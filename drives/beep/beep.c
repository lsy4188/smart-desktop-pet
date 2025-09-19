#include "beep.h"
void init_beep(void)
{
	RCC->AHB1ENR |= 0x01;
	GPIOA->MODER &= ~(0x03<<30);
	GPIOA->MODER |= 0x01<<30;
}
void beep_open(void)
{
	GPIOA->ODR |= (0X01<<15);
}
void beep_close(void)
{
	GPIOA->ODR &= ~(0X01<<15);
}

