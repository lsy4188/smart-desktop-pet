#include "wifi.h"

extern uint8_t usart3_index;
extern uint8_t usart3_isready;
extern uint8_t usart3_arr[256];
void init_wifi_mode(void){
	init_usart3();
	init_usart6();
	send_wifi_command("AT");
	delay_ms(10);
	receve_wifi_response();
}


void send_wifi_command(const char* str){
	strcat((char*)str,"\r\n");
	send_string(USART3, str);
}

void receve_wifi_response(void){
	if(usart3_isready&&strstr((char*)usart3_arr,"OK")!=NULL){
		send_string(USART6, (char*)usart3_arr);
	}else{
		printf("1123456789error....\r\n");
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));
}

uint8_t wait_wifi_response(const char*str,uint32_t timeout){
	uint8_t flg=FAIL;
	while(!usart3_isready){
		if(timeout--==0){
			return FAIL;
		}
	}
	if(strstr((char*)usart3_arr,str)!=NULL){
		flg=SUCCESS;
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));
	return flg;
}


