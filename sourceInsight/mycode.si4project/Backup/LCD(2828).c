#include "lcd.h"

void init_lcd(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	init_tim6_basic();
	init_spi();
	init_usart6();
	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//初始化GPIO
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);	
	//复位
	lcd_reset();
	ST7789V_config();
	//开启背光灯
	LCD_LED_SET;
}

void ST7789V_config(void){
	//ST7789V配置
	lcd_write_command(0x11);  // 退出睡眠模式
	lcd_write_command(0x36);//显存数据访问控制(类似于寄存器地址)
	lcd_write_data(0x00);//配置屏幕的扫描方向、颜色顺序(写寄存器操作)
	lcd_write_command(0x3A);  // 颜色模式
	lcd_write_data(0x55);	  // RGB565格式 (16位)
	//配置屏幕缓冲时间
	lcd_write_command(0xB2);
	lcd_write_data(0x46);//设置正常模式下写入结束后的缓冲时间
	lcd_write_data(0x46);//设置正常模式下写入开始前的缓冲时间
	lcd_write_data(0x01);//设置不同模式下可以使用不同的缓冲时间
	lcd_write_data(0x78);//省电/空闲模式高四位为写入结束后的缓冲时间，低四位写入开始前的缓冲时间
	lcd_write_data(0x78);//局部显示（如只显示一小块区域）时，高四位写入结束后的缓冲时间，低四位写入开始前的缓冲时间
	lcd_write_command(0x20);  // 关闭显示反转
	lcd_write_command(0x29);  // 开启显示
}

void lcd_reset(void){
	LCD_RESET;
	delay_ms(45);
	LCD_NO_RESET;
	delay_ms(45);
}
void lcd_write_command(uint8_t cmd){
	LCD_COMMAND_SET;
	LCD_CS_SET;
	send_data(cmd);
	LCD_CS_CLEAR;
}

void lcd_write_data(uint8_t data){
	LCD_DATA_SET;
	LCD_CS_SET;
	send_data(data);
	LCD_CS_CLEAR;
}
void lcd_set_location(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
	//列地址设置
	lcd_write_command(0x2A);  
    lcd_write_data(x0 >> 8); 
    lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8);  
    lcd_write_data(x1 & 0xFF);
	//行地址设置
    lcd_write_command(0x2B);
    lcd_write_data(y0 >> 8);
    lcd_write_data(y0 & 0xFF);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);
	//写入GRAM
    lcd_write_command(0x2C);
}
void lcd_clear(void){
	uint16_t color=0xFFFF;
	uint32_t i=0;
	lcd_set_location(0, 0, LDC_WIDTH-1, LDC_HEIGHT-1);
	for (i = 0; i < LDC_WIDTH * LDC_HEIGHT; i++) {
		lcd_write_data(color >> 8);    
		lcd_write_data(color & 0xFF);
	}
}
void lcd_fill_color(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color) {
	uint16_t temp=0;
	uint32_t i=0,num=0;
	if(x0>x1){
		temp=x0;
		x0=x1;
		x1=temp;
	}
	if(y0>y1){
		temp=y0;
		y0=y1;
		y1=temp;
	}
	num=(x1-x0+1)*(y1-y0+1);
	lcd_set_location(x0,y0,x1,y1);
	for(i=0;i<num;i++){
		lcd_write_data(color >> 8);
        lcd_write_data(color & 0xFF);
	}
}



