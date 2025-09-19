#include "sg90.h"

ActionMode current_action = STAND; 

/*
flg=0-->后退
flg=1-->前进
2,4号舵机
135度
forty_five_degrees(4);
one_hundred_and_eighty_degrees(4);
45度
one_hundred_and_thirty_five_degrees(4);
one_hundred_and_eighty_degrees(4);
90度
ninety_degrees(4);
one_hundred_and_/eighty_degrees(4);
*/

void init_sg90(uint8_t flg){
	init_tim3_common();
	init_tim6_basic();
	sleep_down(flg);
}

void zero_degrees(uint8_t tim_ch){
	if(tim_ch==1){
		TIM_SetCompare1(TIM3, 500);
	}else if(tim_ch==2){
		TIM_SetCompare2(TIM3, 500);
	}else if(tim_ch==3){
		TIM_SetCompare3(TIM3, 500);
	}else{
		TIM_SetCompare4(TIM3, 500);
	}
}

void forty_five_degrees(uint8_t tim_ch){
	if(tim_ch==1){
		TIM_SetCompare1(TIM3, 1000);
	}else if(tim_ch==2){
		TIM_SetCompare2(TIM3, 1000);
	}else if(tim_ch==3){
		TIM_SetCompare3(TIM3, 1000);
	}else{
		TIM_SetCompare4(TIM3, 1000);
	}
}

void ninety_degrees(uint8_t tim_ch){
	if(tim_ch==1){
		TIM_SetCompare1(TIM3, 1500);
	}else if(tim_ch==2){
		TIM_SetCompare2(TIM3, 1500);
	}else if(tim_ch==3){
		TIM_SetCompare3(TIM3, 1500);
	}else{
		TIM_SetCompare4(TIM3, 1500);
	}
}

void one_hundred_and_thirty_five_degrees(uint8_t tim_ch){
	if(tim_ch==1){
		TIM_SetCompare1(TIM3, 2000);
	}else if(tim_ch==2){
		TIM_SetCompare2(TIM3, 2000);
	}else if(tim_ch==3){
		TIM_SetCompare3(TIM3, 2000);
	}else{
		TIM_SetCompare4(TIM3, 2000);
	}
}

void one_hundred_and_eighty_degrees(uint8_t tim_ch){
	if(tim_ch==1){
		TIM_SetCompare1(TIM3, 2500);
	}else if(tim_ch==2){
		TIM_SetCompare2(TIM3, 2500);
	}else if(tim_ch==3){
		TIM_SetCompare3(TIM3, 2500);
	}else{
		TIM_SetCompare4(TIM3, 2500);
	}
}

void sleep_down(uint8_t flg){
	if(flg){
		zero_degrees(1);
		one_hundred_and_eighty_degrees(2);//前进
		zero_degrees(3);
		one_hundred_and_eighty_degrees(4);
	}else{
		one_hundred_and_eighty_degrees(1);
		zero_degrees(2);//后退
		one_hundred_and_eighty_degrees(3);
		zero_degrees(4);
	}
	delay_ms(450);
	
}



/******************sleep_down(1)*******************/
void stand_up(void){
	ninety_degrees(1);
	
	ninety_degrees(2);
		
	ninety_degrees(3);
		
	ninety_degrees(4);
	
	delay_ms(10);
}


void seat_squat(void){
	ninety_degrees(1);
	
	ninety_degrees(2);

	zero_degrees(3);

	one_hundred_and_eighty_degrees(4);
	
	delay_ms(10);

}

void turn_left(void){
	one_hundred_and_thirty_five_degrees(1);
	one_hundred_and_thirty_five_degrees(3);
	delay_ms(100);
	ninety_degrees(2);
	ninety_degrees(4);
	delay_ms(10);
	
	zero_degrees(1);
	zero_degrees(3);
	delay_ms(250);
	one_hundred_and_eighty_degrees(2);
	one_hundred_and_eighty_degrees(4);
	delay_ms(250);
}


void turn_right(void){
    ninety_degrees(1);
    ninety_degrees(2);
    ninety_degrees(3);
    ninety_degrees(4);
    delay_ms(100);


	ninety_degrees(2);
	ninety_degrees(4);
	delay_ms(150);
	one_hundred_and_thirty_five_degrees(1);
	one_hundred_and_thirty_five_degrees(3);
	delay_ms(200);
	
	zero_degrees(1);
	zero_degrees(3);
	one_hundred_and_eighty_degrees(2);
	one_hundred_and_eighty_degrees(4);
	delay_ms(100);


}

void forward(void){
	ninety_degrees(1);//左前肢
	ninety_degrees(4);//右后肢
	delay_ms(300);
	
	zero_degrees(1);
	one_hundred_and_eighty_degrees(4);
	delay_ms(300);
	
	ninety_degrees(2);//右前肢
	ninety_degrees(3);//左后肢
	delay_ms(300);
	
	one_hundred_and_eighty_degrees(2);
	zero_degrees(3);
	delay_ms(300);
	
}
/******************sleep_down(0)*******************/
void back(void){
	ninety_degrees(2);
	ninety_degrees(3);
	delay_ms(300);
	
	zero_degrees(2);
	one_hundred_and_eighty_degrees(3);
	delay_ms(300);

	ninety_degrees(1);
	ninety_degrees(4);
	delay_ms(300);
	
	one_hundred_and_eighty_degrees(1);
	zero_degrees(4);
	delay_ms(300);
}
/****************语音模块函数********************/
void update_action(void){
    static ActionMode last_action = STAND;
    if (last_action != current_action) {
        last_action = current_action;
    }
    switch (current_action) {
        case FORWARD:
            forward();
            break;
        case BACK:
            back();
            break;
        case TURN_LEFT:
			turn_left();
            break;
		case TURN_RIGHT:
			turn_right();
            break;
		case STAND:
			stand_up();
            break;
        case SEAT_SQUAT:
			seat_squat();
            break;
    }

}

void forward_action(void){
	current_action=FORWARD;
}
void back_action(void){
	current_action=BACK;
}
void turn_left_action(void){
	current_action=TURN_LEFT;
}

void turn_right_action(void){
	current_action=TURN_RIGHT;
}

void stand_action(void){
	current_action=STAND;
}

void seat_squat_action(void){
	current_action=SEAT_SQUAT;
}



