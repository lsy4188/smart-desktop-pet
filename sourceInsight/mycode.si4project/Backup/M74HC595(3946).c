#include "M74HC595.h"

DisplayMode current_mode = DISPLAY_OFF; 

//段选
const uint8_t seg[]={
	0x3F, // "0"
	0x06, // "1"
	0x5B, // "2"
	0x4F, // "3"
	0x66, // "4"
	0x6D, // "5"
	0x7D, // "6"
	0x07, // "7"
	0x7F, // "8"
	0x6F, // "9"
	0x77, // "A"
	0x7C, // "B"
	0x39, // "C"
	0x5E, // "D"
	0x79, // "E"
	0x71, // "F"
	0x80, //"."
	0x00//"熄灭"
};
//位选
const uint8_t pos[]={
	0x01, 
	0x02, 
	0x04, 
	0x08, 
};

void init_M74HC595(void){
	init_tim2_common();//温湿度
	init_tim6_basic();//延时
	//init_SHT20();由于MAX30102和SHT20的I2C会有覆盖的可能不能在此处初始化
	init_usart6();
	init_spi();
}

void display_data(uint8_t which,uint8_t data){
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//拉低NSS开始通讯
	send_data(pos[which]);
	send_data(seg[data]);
	delay_us(100);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高NSS结束通讯
}

extern uint8_t sensor[5];
/*void display_sensor(double num){
		int i=0,j=0;
		int data;
		char str[10]={0};
		memset(sensor,0,sizeof(sensor));
		sprintf(str,"%.2f",num);
		for (i = 0; str[i] != '\0'; i++) {
			if (isdigit(str[i])) {
				data = str[i] - '0';
			}else{
				data=16;
			}
			sensor[i]=data;
		}		
		for(i=0,j=0;i<4;i++,j++){
			display_data(i,sensor[j]);	
			if(i!=3&&sensor[i+1]==16){
				display_data(i,sensor[++j]);
			}
		}
}*/

uint8_t sht20_data[4];
void display_sensor(double num){
	int i=0,j=0;
	uint8_t sensor_size=4;
	int data;
	char str[10]={0};
	memset(sht20_data,0,sizeof(sht20_data));
	sprintf(str,"%.2f",num);
	for (i = 0; str[i] != '\0'&&j < sensor_size; i++) {
		if (isdigit((unsigned char)str[i])) {
           data = seg[str[i] - '0'];
           if (str[i+1] == '.') {
               data |= seg[16];
               i++; 
           }
           sht20_data[j++] = data;
        }
	}	
}


/***********************下面函数用于声控*********************************/
void set_display_mode(DisplayMode mode){
	current_mode=mode;
	if(current_mode==DISPLAY_OFF){
		clear_display();
	}

}
DisplayMode get_display_mode(void){
	return current_mode;

}
void clear_display(void){
	uint8_t i=0;
	for(i=0;i<4;i++){
		display_data(i,17);
	}
}
void update_display(void){
    static DisplayMode last_mode = DISPLAY_OFF;
    if (last_mode != current_mode) {
        clear_display();
        last_mode = current_mode;
    }
    switch (current_mode) {
        case TEMP_DISPLAY:
            display_sht20_tem();
            break;
        case HUMI_DISPLAY:
            display_sht20_rh();
            break;
		case SPO2_DISPLAY:
            display_MAX30102_spo2();
            break;
        case DISPLAY_OFF:
            break;
    }

}


extern int32_t spo2;
void display_MAX30102_spo2(void) {
    if (current_mode != SPO2_DISPLAY) return;
    display_sensor((double)spo2);
}

extern sht20 sh;
void display_sht20_tem(void){
	if(current_mode!=TEMP_DISPLAY) return;
	if(sh.flg){
		sh.flg=0;
		Process_Data("hold");
	}
	display_sensor(sh.tem);
}

void display_sht20_rh(void){
	if(current_mode!=HUMI_DISPLAY) return;
	if(sh.flg){
			sh.flg=0;
			Process_Data("hold");
		}
		display_sensor(sh.rh);
}

void display_tem(void){
	init_SHT20();
	set_display_mode(TEMP_DISPLAY);
}
void display_rh(void){
	init_SHT20();
	set_display_mode(HUMI_DISPLAY);
}
void display_spo2(void) {
    set_display_mode(SPO2_DISPLAY);
}

void display_close(void){
	set_display_mode(DISPLAY_OFF);
}


