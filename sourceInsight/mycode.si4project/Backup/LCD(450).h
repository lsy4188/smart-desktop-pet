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


#define LCD_CS_SET GPIO_ResetBits(GPIOB, GPIO_Pin_12);
#define LCD_CS_CLEAR GPIO_SetBits(GPIOB, GPIO_Pin_12);

#define LCD_RESET GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define LCD_NO_RESET GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define LCD_DATA_SET GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define LCD_COMMAND_SET GPIO_ResetBits(GPIOA, GPIO_Pin_9)

#define LCD_LED_SET GPIO_SetBits(GPIOA, GPIO_Pin_10)
#define LCD_LED_NO GPIO_ResetBits(GPIOA, GPIO_Pin_10)

typedef enum{
	LCD_PORTRAIT = 0,	   // 竖屏
	LCD_LANDSCAPE = 1,	  // 横屏
}LCD_Direction;
	
extern LCD_Direction lcd_direction;

/*
HS280S010B显示屏初始化
*/
void init_lcd(LCD_Direction direction);
/*
配置HS280S010B驱动配置
*/
void ST7789V_config(LCD_Direction direction);
/*
硬件复位
*/
void lcd_reset(void);
/*
清屏
*/
void lcd_clear(void);
/*
写指令
*/
void lcd_write_command(uint8_t cmd);
/*
写数据
*/
void lcd_write_data(uint8_t data);
/*
设置区域
*/
void lcd_set_location(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
/*
设置区域填充指定颜色
*/
void lcd_fill_color(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color);
/*
在指定位置写指定颜色和大小的字符
*/
void lcd_write_char(uint16_t x,uint16_t y,char ch, uint16_t color, uint16_t back_color, uint8_t size);
/*
在指定位置写指定颜色和大小的字符串
*/
void lcd_write_str(uint16_t x,uint16_t y,const char *str, uint16_t color, uint16_t back_color, uint8_t size);
/*
获取中文映射索引
*/
int find_CH_index(const char*ch);
/*
写num个中文字符
*/
void lcd_write_CH(uint16_t x, uint16_t y, const uint8_t * data,uint8_t num ,uint16_t color, uint16_t back_color, uint8_t size);


#endif

