#include "wifi.h"

extern uint8_t usart3_index;
extern uint8_t usart3_isready;
extern uint8_t usart3_arr[256];
void init_wifi_mode(void){
	init_usart3();
	init_usart6();
	init_tim6_basic();
	send_wifi_command("AT");
	delay_ms(3000);
	receve_wifi_response("OK");
	
	send_wifi_command("AT+RESTORE");
	delay_ms(4000);
	receve_wifi_response("OK");
	
	send_wifi_command("AT+CWMODE=1");
	delay_ms(1000);
	receve_wifi_response("OK");

	send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
	delay_ms(8000);
	receve_wifi_response("OK");
}


void send_wifi_command(char* str){
    char buffer[256];  
    sprintf(buffer, "%s\r\n", str);  
	send_string(USART3, buffer);
}

void receve_wifi_response(const char* expected){
	if(usart3_isready&&strstr((char*)usart3_arr,expected)!=NULL){
		send_string(USART6, (char*)usart3_arr);
	}else{
		printf("connecting..........\r\n");
	}
	usart3_index=0;
	usart3_isready=0;
	memset(usart3_arr,0,sizeof(usart3_arr));
}

void send_tcp_data(const char*server,uint16_t port, const char* data){
	char cmd[128];
	memset(cmd,0,sizeof(cmd));
	//建立TCP连接
	sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d",server,port);
	send_wifi_command(cmd);
	delay_ms(8000);
	receve_wifi_response("OK");
	//传输数据
	sprintf(cmd,"AT+CIPSEND=%d",strlen(data));
	send_wifi_command(cmd);
	delay_ms(3000);
	receve_wifi_response("OK");
	send_wifi_command((char*)data);
	delay_ms(2000);
	//中断传输
	send_wifi_command("AT+CIPCLOSE");
	delay_ms(4000);
	receve_wifi_response("OK");
}



