#include "lcd.h"

void init_lcd(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	init_tim6_basic();
	init_spi();
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
	lcd_write_command(0x36);//显存数据访问控制(类似于寄存器地址)
	lcd_write_data(0x00);//配置屏幕的扫描方向、颜色顺序(写寄存器操作)
	lcd_write_command(0x3A);  // 颜色模式
    lcd_write_data(0x55);     // RGB565格式 (16位)
    lcd_write_command(0x20);  // 关闭显示反转
    lcd_write_command(0x11);  // 退出睡眠模式
    delay_ms(120);
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
	lcd_write_command(0x2A);  // 列地址设置
    lcd_write_data(x0 >> 8);  // 起始列高8位
    lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8);  // 结束列高8位
    lcd_write_data(x1 & 0xFF);

    lcd_write_command(0x2B);  // 行地址设置
    lcd_write_data(y0 >> 8);  // 起始行高8位
    lcd_write_data(y0 & 0xFF);
    lcd_write_data(y1 >> 8);  // 结束行高8位
    lcd_write_data(y1 & 0xFF);

    lcd_write_command(0x2C);  // 写入GRAM
}

void lcd_fill_color(uint16_t color) {
	uint32_t i=0;
    lcd_set_location(0, 0, 239, 319);
    for ( i = 0; i < 240 * 320; i++) {
        lcd_write_data(color >> 8);    // 高8位
        lcd_write_data(color & 0xFF);  // 低8位
    }
}


