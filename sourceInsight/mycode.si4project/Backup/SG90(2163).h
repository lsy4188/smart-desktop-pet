#ifndef __SG90_H__
#define __SG90_H__
#include <stm32f4xx.h>
#include "tim.h"

typedef enum {
	STAND,			//站立
   	FORWARD,  	 	// 前进
    BACK,   		// 后退
    TURN_LEFT,    	// 左转
    TURN_RIGHT,    	// 左转
    SEAT_SQUAT		//蹲下
} ActionMode;

// 声明全局动作模式变量
extern ActionMode current_action;


void init_sg90(uint8_t flg);

void zero_degrees(uint8_t tim_ch);

void forty_five_degrees(uint8_t tim_ch);

void ninety_degrees(uint8_t tim_ch);

void one_hundred_and_thirty_five_degrees(uint8_t tim_ch);

void one_hundred_and_eighty_degrees(uint8_t tim_ch);

/*
站立
*/
void stand_up(void);
/*
趴下
*/
void sleep_down(uint8_t flg);
/*
前进
*/
void forward(void);
/*
后退
*/
void back(void);
/*
坐下
*/
void seat_squat(void);

/*
左转
*/
void turn_left(void);

/*
右转
*/
void turn_right(void);



//流程控制函数
void update_action(void);
void forward_action(void);
void back_action(void);
void turn_left_action(void);
void turn_right_action(void);
void stand_action(void);
void seat_squat_action(void);

#endif

