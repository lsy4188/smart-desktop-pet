#define FONT8x16_IMPLEMENTATION
#include "lcd.h"

LCD_Direction lcd_direction=LCD_PORTRAIT;
uint16_t lcd_width=240;
uint16_t lcd_height=320;

/***********************显示******************************/
void init_lcd(LCD_Direction direction){
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
	ST7789V_config(direction);
	//开启背光灯
	LCD_LED_SET;
}

void ST7789V_config(LCD_Direction direction){
	//ST7789V配置
	lcd_write_command(0x11);  // 退出睡眠模式
	lcd_write_command(0x36);//显存数据访问控制(类似于寄存器地址)
	if(direction==LCD_PORTRAIT){
		lcd_write_data(0x00);//配置屏幕方向(竖屏)、颜色顺序(写寄存器操作)
		
	}else{
		lcd_write_data(0x60);//配置屏幕方向(横屏)、颜色顺序(写寄存器操作)
		lcd_direction=LCD_LANDSCAPE;
		lcd_width=320;
		lcd_height=240;
	}
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
	//列地址设置(水平方向显示的像素)
	lcd_write_command(0x2A);  
    lcd_write_data(x0 >> 8); 
    lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8);  
    lcd_write_data(x1 & 0xFF);
	//行地址设置(垂直方向显示的像素)
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
	lcd_set_location(0, 0, lcd_width-1, lcd_height-1);
	for (i = 0; i < lcd_width * lcd_height; i++) {
		lcd_write_data(color >> 8);    
		lcd_write_data(color & 0xFF);
	}
}

void lcd_write_char(uint16_t x,uint16_t y,char ch, uint16_t color, uint16_t back_color, uint8_t size){
	uint8_t row=0,i=0,col=0,j=0,pixelData=0;
	const uint8_t* lineData=NULL;
	if (ch < 32 || ch > 126||size<1) ch = ' ';
	lcd_set_location(x,y,(x+8*size)-1,(y+16*size)-1);
	lineData = font8x16[ch];
	//遍历字符集(行)
	for(row=0;row<16;row++){
		//垂直放大
		for(i=0;i<size;i++){
			//获取像素
			pixelData=lineData[row];
			//遍历当前行的8列(列)
			for(col=0;col<8;col++){
				//水平放大
				for(j=0;j<size;j++){
					//判断当前像素是否点亮
                    if (pixelData & 0x80) {
                        // 发送字符颜色
                        lcd_write_data(color>>8);
                        lcd_write_data(color&0xFF);
                    } else {
                        // 发送背景颜色
                        lcd_write_data(back_color>>8);
                        lcd_write_data(back_color&0xFF);
                    }
				}
				// 处理下一列像素(左移1位，检查下一个bit)
                pixelData <<= 1;
			}
		}
	}
}

void lcd_write_str(uint16_t x,uint16_t y,const char *str, uint16_t color, uint16_t back_color, uint8_t size){
	uint16_t startx=x;
	while(*str){
		if(startx+8*size>lcd_width){//判断是否需要换行
			startx=x;
			y+=16*size;
			if(y+16*size>lcd_height){ // 超出屏幕范围，终止显示
				break;
			}
		}
		lcd_write_char(startx,y,*str,color,back_color,size);
		startx+=8*size;
		str++;
	}
}

void lcd_write_CH(uint16_t x, uint16_t y, const uint8_t * data,uint8_t num ,uint16_t color, uint16_t back_color, uint8_t size){
	uint8_t i=0,row=0,col=0,left_byte=0,right_byte=0,v_scale=0,h_scale=0,is_pixel=0;
	uint16_t row_data_offset=0;
	if(num<=0) num=1;
	if(size<=0)size=1;
	for(i=0;i<num;i++){
		lcd_set_location(x+i*16*size,y,((x+i*16*size)+16*size)-1,(y+16*size)-1);
		for(row=0;row<16;row++){//行
			row_data_offset=row*num*2;//当前行在数据数组中的偏移(每行有两个字节（16位），分别表示左8列和右8列)
			left_byte=data[row_data_offset+i*2];//左8列
			right_byte=data[row_data_offset+i*2+1];//右8列
			//垂直放大
			for(v_scale=0;v_scale<size;v_scale++){
				//绘制左8列（原始列0~7）
				for(col=0;col<8;col++){
					//判断像素是否需要点亮
					is_pixel=(left_byte&(0x80>>col))?1:0;
					//水平放大
					for(h_scale=0;h_scale<size;h_scale++){
						if(is_pixel){
							lcd_write_data(color >> 8);
                            lcd_write_data(color & 0xFF);
						}else{
							lcd_write_data(back_color >> 8);
                            lcd_write_data(back_color & 0xFF);
						}
					}
				}
				//绘制右8列(原始列8~15）
				for(col=0;col<8;col++){
					is_pixel=(right_byte&(0x80>>col))?1:0;
					//水平放大
					for(h_scale=0;h_scale<size;h_scale++){
						if(is_pixel){
							lcd_write_data(color >> 8);
                            lcd_write_data(color & 0xFF);
						}else{
							lcd_write_data(back_color >> 8);
                            lcd_write_data(back_color & 0xFF);
						}
					}
				}
			}
		}
	}
}

/*****************触摸**********************/



