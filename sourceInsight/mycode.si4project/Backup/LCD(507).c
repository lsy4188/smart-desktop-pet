#define FONT8x16_IMPLEMENTATION
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
	lcd_write_data(0x00);//配置屏幕方向(竖屏)、颜色顺序(写寄存器操作)
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
		if(startx+8*size>LDC_WIDTH){//判断是否需要换行
			startx=x;
			y+=16*size;
			if(y+16*size>LDC_HEIGHT){ // 超出屏幕范围，终止显示
				break;
			}
		}
		lcd_write_char(startx,y,*str,color,back_color,size);
		startx+=8*size;
		str++;
	}
}

uint8_t find_CH_index(const char * ch){
	uint8_t i=0;
	for(i=0;i<sizeof(chileselib)/sizeof(ChineseChar);i++){
		if(chileselib[i].gbk[0]==(uint8_t)ch[0]&&chileselib[i].gbk[1]==(uint8_t)ch[1]){
			return i;
		}
	}
	return -1;
}

void lcd_write_CH(uint16_t x, uint16_t y, const uint8_t * data, uint16_t color, uint16_t back_color, uint8_t size){
	uint8_t col=0,seg=0,bit=0,pixelData=0,i=0,j=0;
	lcd_set_location(x,y,(x+8*size)-1,(y+32*size)-1);
	//遍历8列
	for(col=0;col<8;col++){
		// 遍历当前列的4个字节段（32行 = 4段×8行）
		for(seg=0;seg<4;seg++){
			//获取当前列、当前段的点阵数据（索引计算：列×4 + 段）
			pixelData=data[col*4+seg];
			//处理当前列数据的8个像素
			for(bit=0;bit<8;bit++){
				//垂直方向放大
				for(i=0;i<size;i++){
					//水平方向放大
					for(j=0;j<size;j++){
						// 判断当前像素是否点亮
                        if (pixelData & 0x80) {
                            // 发送字符颜色
                            lcd_write_data(color >> 8);
                            lcd_write_data(color & 0xFF);
                        } else {
                            // 发送背景颜色
                            lcd_write_data(back_color >> 8);
                            lcd_write_data(back_color & 0xFF);
                        }
					}
				}
				// 处理下一列像素
                pixelData <<= 1;
			}
		}
	}
}
void lcd_write_mix_language(uint16_t x,uint16_t y,const char *str, uint16_t color, uint16_t back_color, uint8_t size){
	uint16_t startx=x;
	uint8_t index=0;
	
	while(*str){
		//判断是中文还是ASCII字符
		if((uint8_t)*str>=0x80){
			//中文字符需要占两个字节
			char temp[3]={str[0],str[1],'\0'};
			index=find_CH_index(temp);
			if(index>=0){
				if(startx+8*size>LDC_WIDTH){
					startx=x;
					y+=32*size;
					if(y+32*size>LDC_HEIGHT){ // 超出屏幕范围，终止显示
						break;
					}
				}
				lcd_write_CH(startx,y, chileselib[index].data, color,back_color,size);
			}else{
				printf("%s does not exist\r\n",*str);
			}
			str+=2;
			startx+=8*size;//每个汉字宽度8像素
		}else{
			if(startx+8*size>LDC_WIDTH){//判断是否需要换行
			startx=x;
			y+=16*size;
			if(y+16*size>LDC_HEIGHT){ // 超出屏幕范围，终止显示
				break;
			}
		}
		lcd_write_char(startx,y,*str,color,back_color,size);
		startx+=8*size;
		str++;
		}
	}
}



