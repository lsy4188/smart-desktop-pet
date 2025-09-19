#include "wifi.h"

extern uint8_t usart3_index;
extern uint8_t usart3_isready;
extern uint8_t usart3_arr[256];
void init_wifi_mode(void){
	init_usart3();
	init_usart6();
	send_wifi_command("AT");
	receve_wifi_response();
	send_wifi_command("AT+RESTORE");
	receve_wifi_response();
	send_wifi_command("AT+CWMODE=1");
	receve_wifi_response(); 
	send_wifi_command("AT+CWJAP_DEF='hqyjcs','hqyj2022'");
	receve_wifi_response();
	send_wifi_command("AT+CIPSTART='TCP','192.168.20.52',8080");
	receve_wifi_response();
	send_wifi_command("AT+CIPSEND=10");
	receve_wifi_response();
}


void send_wifi_command(char* str){
    char buffer[256];  
    sprintf(buffer, "%s\r\n", str);  
	send_string(USART3, buffer);
}

void receve_wifi_response(void){
	while(!usart3_isready||strstr((char*)usart3_arr,"OK")==NULL);
	send_string(USART6, (char*)usart3_arr);
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


