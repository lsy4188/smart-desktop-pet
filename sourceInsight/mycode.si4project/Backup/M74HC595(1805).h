#ifndef __M74HC595_H__
#define __M74HC595_H__
#include <stm32f4xx.h>
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "sht20.h"
#include <ctype.h>
#include <string.h>

typedef enum {
    TEMP_DISPLAY,   // 温度显示模式
    HUMI_DISPLAY,   // 湿度显示模式
    SPO2_DISPLAY,   // 血氧显示模式
    DISPLAY_OFF     // 关闭显示模式
} DisplayMode;

// 声明全局显示模式变量
extern DisplayMode current_mode;

//基本函数
void init_M74HC595(void);
/**************************************
display_data
函数功能：which-选择的数码管；data-显示的数据
****************************************/
void display_data(uint8_t which,uint8_t data);

void display_sensor(double num);

void display_sht20_tem(void);

void display_sht20_rh(void);

void display_MAX30102_spo2(void);



// 声明显示控制函数
void set_display_mode(DisplayMode mode);
DisplayMode get_display_mode(void);
void clear_display(void);
void update_display(void);


//流程控制函数
void display_tem(void);
void display_rh(void);
void display_spo2(void);
void display_close(void);






#endif

