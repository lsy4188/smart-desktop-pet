#include "wifi.h"

extern wifi_buffer wb;
void init_wifi_mode(void){
	init_tim6_basic();
	init_usart3();
	init_usart6();
	init_led();
	init_beep();
	init_fan();
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

uint8_t receve_wifi_response(const char* expected){
	char device_name[10]={0};
	char state[10]={0};
	char* cmd_ctrl=NULL;
	if(wb.is_ready){
		send_string(USART6, (char*)wb.usart3_arr);
		cmd_ctrl=strstr((char*)wb.usart3_arr,"CTRL");
		if(cmd_ctrl!=NULL){
         if(sscanf(cmd_ctrl, "CTRL:%s %s", device_name, state) == 2) {
              control_device(device_name, state);
          }
		}
        if(strstr((char*)wb.usart3_arr, expected) != NULL) {
			wb.index= 0;
			wb.is_ready = 0;
			memset(wb.usart3_arr, 0, sizeof(wb.usart3_arr));
            return SUCCESS;
        }
	}
    wb.index= 0;
    wb.is_ready = 0;
    memset(wb.usart3_arr, 0, sizeof(wb.usart3_arr));
    return FAIL;
}

uint8_t receve_wifi_response(const char* expected){
	if(wb.is_ready){
		
	}
    wb.index= 0;
    wb.is_ready = 0;
    memset(wb.usart3_arr, 0, sizeof(wb.usart3_arr));
    return FAIL;
}


void control_device(char* device_name, char* state){
	if(strcmp(device_name,"led")==0){
		if(strcmp(state,"open")==0){
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		}else {
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		}
	}else if(strcmp(device_name,"beep")==0){
		if(strcmp(state,"open")==0){
			GPIO_SetBits(GPIOA, GPIO_Pin_15);
		}else {
			GPIO_ResetBits(GPIOA, GPIO_Pin_15);
		}

	}else if(strcmp(device_name,"fan")==0){
		if(strcmp(state,"open")==0){
			GPIO_SetBits(GPIOC, GPIO_Pin_6);
		}else {
			GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		}
	}else{
		send_string(USART6, "device unknow....\r\n");
	}
}


void create_tcp_connect(const char*server,uint16_t port, const char* data){
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
}


void destory_tcp_connect(void){
	send_wifi_command("AT+CIPCLOSE");
	delay_ms(4000);
	receve_wifi_response("OK");
}


