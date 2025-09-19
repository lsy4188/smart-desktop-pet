#include "wifi.h"

extern uint8_t usart3_index;
extern uint8_t usart3_isready;
extern uint8_t usart3_arr[256];
extern uint8_t temp;
void init_wifi_mode(void){
	init_usart3();
	init_usart6();
	send_wifi_command("AT+RESTORE");
	//receve_wifi_responseV1("OK",3000);
	delay_ms(3000);
	receve_wifi_response();
	printf("AT+RESTORE==%d\r\n",temp);
	
	send_wifi_command("AT+CWMODE=1");
	//receve_wifi_responseV1("OK",3000);
	delay_ms(3000);
	receve_wifi_response();

	send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
	//receve_wifi_responseV1("OK",8000);
	delay_ms(8000);
	receve_wifi_response();

	send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
	//receve_wifi_responseV1("OK",7000);
	delay_ms(7000);
	receve_wifi_response();

	send_wifi_command("AT+CIPSEND=10");
	//receve_wifi_responseV1("OK",3000);
	delay_ms(7000);
	receve_wifi_response();
}


void send_wifi_command(char* str){
    char buffer[256];  
    sprintf(buffer, "%s\r\n", str);  
	send_string(USART3, buffer);
}

void receve_wifi_response(void){
	if(usart3_isready&&strstr((char*)usart3_arr,expected)!=NULL){
		send_string(USART6, (char*)usart3_arr);
	}else{
		printf("connecting..........\r\n");
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));

}
uint8_t receve_wifi_responseV1(const char* expected, uint16_t timeout){
	uint8_t time=0;
	while(strstr((char*)usart3_arr,"OK")==NULL){
		delay_ms(2);
		if(++time>timeout){
			send_string(USART6, "connect out time.....\r\n");
			return FAIL;
		}
	}
	//delay_ms(100);
	send_string(USART6, (char*)usart3_arr);
	send_string(USART6, "123\r\n");
	if(strstr((char*)usart3_arr,expected)!=NULL){
		send_string(USART6, (char*)usart3_arr);
	}else{
		send_string(USART6, "expected error...\r\n");
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));
	return SUCCESS;
}



