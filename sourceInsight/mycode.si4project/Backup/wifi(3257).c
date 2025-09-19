#include "wifi.h"

extern uint8_t usart3_index;
extern uint8_t usart3_isready;
extern uint8_t usart3_arr[256];
void init_wifi_mode(void){
	init_usart3();
	init_usart6();
	send_wifi_command("AT");
	wait_wifi_response("OK",3000);
	//delay_ms(3000);
	//receve_wifi_response();
	send_wifi_command("AT+RESTORE");
	wait_wifi_response("OK",3000);
	//delay_ms(3000);
	//receve_wifi_response();
	send_wifi_command("AT+CWMODE=1");
	wait_wifi_response("OK",3000);
	//delay_ms(3000);
	//receve_wifi_response(); 
	send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
	wait_wifi_response("OK",3000);
	//delay_ms(9000);
	//receve_wifi_response();
	send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
	wait_wifi_response("OK",3000);
	//delay_ms(8000);
	//receve_wifi_response();
	send_wifi_command("AT+CIPSEND=10");
	wait_wifi_response("OK",3000);
	//delay_ms(3000);
	//receve_wifi_response();
}


void send_wifi_command(char* str){
    char buffer[256];  
    sprintf(buffer, "%s\r\n", str);  
	send_string(USART3, buffer);
}

uint8_t wait_wifi_response(const char* expected, uint32_t timeout){
	uint32_t time=0;
	while(!usart3_isready){
		delay_ms(1);
		if(++time>timeout){
			return FAIL;
		}
	}
	if(strstr((char*)usart3_arr,expected)!=NULL){
		return SUCCESS;
	}
	return FAIL;
}
void receve_wifi_response(void){
	if(usart3_isready&&strstr((char*)usart3_arr,"OK")!=NULL){
		send_string(USART6, (char*)usart3_arr);
	}else{
		printf("connecting..........\r\n");
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));

}


