#include "wifi.h"

void init_wifi_mode(void){
	init_usart3();
}


void send_wifi_result(const char* str){
	send_string(USART3, str);
}


extern uint8_t usart3_index;
extern uint8_t usart3_isready;
extern uint8_t usart3_arr[256];
uint8_t receve_wifi_data(void){
	uint8_t flg=0;
	if(usart3_isready){
		printf("%s\r\n",usart3_arr);
		if(strcmp((char*)usart3_arr,"ok")==0){
			flg=SUCCESS; 
		}
	}else{
		flg=FAIL;
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));
	return flg;
}


