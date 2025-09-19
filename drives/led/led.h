#ifndef __LED_H__
#define __LED_H__
#include <stm32f4xx.h>
#define LED1 GPIO_Pin_4
#define LED2 GPIO_Pin_3
#define LED3 GPIO_Pin_13
void init_led(void);

void turn_on_all_led(void);

void turn_off_all_led(void);

void turn_on_ledx(u16 led);

void turn_off_ledx(u16 led);

void toggle_ledx(u16 led);


#endif

