#ifndef __LCD_H__
#define __LCD_H__
#include <stm32f4xx.h>
#include "tim.h"
#include "spi.h"
#include "usart.h"
#include "font8x16.h"
/*
RGB565的颜色展示
CS-PB12
RST-PA8
D/C-PA9
SID-PB15
SCL-PB13
LED-PA10
SDO-PB14
*/

#define LDC_WIDTH 240
#define LDC_HEIGHT 320


#define LCD_CS_SET GPIO_ResetBits(GPIOB, GPIO_Pin_12);
#define LCD_CS_CLEAR GPIO_SetBits(GPIOB, GPIO_Pin_12);

#define LCD_RESET GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define LCD_NO_RESET GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define LCD_DATA_SET GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define LCD_COMMAND_SET GPIO_ResetBits(GPIOA, GPIO_Pin_9)

#define LCD_LED_SET GPIO_SetBits(GPIOA, GPIO_Pin_10)
#define LCD_LED_NO GPIO_ResetBits(GPIOA, GPIO_Pin_10)


void init_lcd(void);
void ST7789V_config(void);
void lcd_reset(void);
void lcd_clear(void);
void lcd_write_command(uint8_t cmd);
void lcd_write_data(uint8_t data);
void lcd_set_location(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
void lcd_fill_color(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color);
void lcd_write_char(uint16_t x,uint16_t y,char ch, uint16_t color, uint16_t back_color, uint8_t size);




#endif

