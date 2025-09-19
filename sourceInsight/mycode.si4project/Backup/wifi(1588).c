
#include "wifi.h"


/*
wifi模组
*/
wifi_buffer wb={0};
WIFI_MODE wifi_mode=DEFAULT;
void init_wifi_mode(void){
	init_tim7_basic();
	init_usart3();
	init_usart6();
	init_led();
	init_beep();
	init_fan();
    wb.connect_state = AT_RST;
	wb.disconnect_state=DESTORY_TCP_CONNECT;
    wb.timeout_counter = 0;
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
    return FAIL;
}


void wifi_connect_process(void){
	switch(wb.connect_state){
		case AT_RST:
			if(receve_wifi_response("ready")){
				wb.connect_state=WIFI_STATE_AT;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+RST");
				}else{
					wb.connect_state=WIFI_ERROR;
				}	
			}
			break;
		case WIFI_STATE_AT:
			if(receve_wifi_response("OK")){
				wb.connect_state=WIFI_STATE_CWMODE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWMODE=1");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT");
				}else{
					wb.connect_state=WIFI_ERROR;
				}	
			}
			break;	
		case WIFI_STATE_CWMODE: 
			if(receve_wifi_response("OK")){
				wb.connect_state=WIFI_STATE_CWJAP;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWJAP_DEF=\"CMCC-1209\",\"13787176367\"");
			}else if(wb.timeout_counter>6000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWMODE=1");
				}else{
					wb.connect_state=WIFI_ERROR;
				}
			}
			break;	
		case WIFI_STATE_CWJAP: 
			if (receve_wifi_response("OK")) {
                wb.connect_state = CREATE_TCP_CONNECT;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.1.7\",8080");
            } else if (wb.timeout_counter > 8000) {
                if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWJAP_DEF=\"CMCC-1209\",\"13787176367\"");
				}else{
					wb.connect_state=WIFI_ERROR;
				}
            }
			break;
		case CREATE_TCP_CONNECT: 
			if (receve_wifi_response("OK")) {
                wb.connect_state = WIFI_DONE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				wb.is_connect=1;
            } else if (wb.timeout_counter > 9000) {
                if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.1.7\",8080");
				}else{
					wb.connect_state=WIFI_ERROR;
				}
            }
			break;
		case WIFI_DONE: 
			receve_wifi_response("OK");
			break;
			
		case WIFI_ERROR: 
			connect_error();	
			break;		
	}
}


void wifi_disconnect_process(void){
	switch(wb.disconnect_state){
		case DESTORY_TCP_CONNECT:
			if(receve_wifi_response("OK")){
 				wb.disconnect_state=DISCONNECT_WIFI;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWQAP");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CIPCLOSE");
				}else{
					wb.disconnect_state=DISCONNECT_WIFI_ERROR;
				}	
			}
			break;
		case DISCONNECT_WIFI:
			if(receve_wifi_response("OK")){
				wb.disconnect_state=WIFI_RESTORE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+RESTORE");
			}else if(wb.timeout_counter>4000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWQAP");
				}else{
					wb.disconnect_state=DISCONNECT_WIFI_ERROR;
				}	
			}
			break;
		case WIFI_RESTORE:
			if(receve_wifi_response("ready")){
				wb.timeout_counter = 0;
				wb.retry_count=0;
				wb.connect_state = AT_RST;
				wb.disconnect_state=DESTORY_TCP_CONNECT;
				wifi_mode=DEFAULT;
			}else if(wb.timeout_counter>4000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+RESTORE");
				}else{
					wb.disconnect_state=DISCONNECT_WIFI_ERROR;
				}
			}
			break;	
		case DISCONNECT_WIFI_ERROR: 
			disconnect_error();
			break;		
		}


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

void connect_error(void){
	static uint32_t connect_error_time=0;
	GPIO_SetBits(GPIOC, LED1);
	if(connect_error_time++>5000){
		connect_error_time=0;
		wifi_mode=DEFAULT;
		reset_wifi_state();

	}
}
void disconnect_error(void){
	static uint32_t disconnect_error_time=0;
	GPIO_SetBits(GPIOC, LED2);
	if(disconnect_error_time++>5000){
		disconnect_error_time=0;
		wifi_mode=DEFAULT;
		reset_wifi_state();
	}
}


/******************以下函数用于语音控制*****************************/
void update_wifi_mode(void){
    static WIFI_MODE last_mode = DEFAULT;
    if (last_mode != wifi_mode) {
        last_mode = wifi_mode;
        reset_wifi_state();
    }
    switch (wifi_mode) {
        case CONNECT:
            wifi_connect_process();
            break;
        case DISCONNECT:
			wifi_disconnect_process();
            break;
		 case DEFAULT:
            break;
    }
}

void reset_wifi_state(void){
    wb.connect_state = AT_RST;
	wb.disconnect_state=DESTORY_TCP_CONNECT;
    wb.timeout_counter = 0;
	wb.retry_count=0;
	wb.index= 0;
	wb.is_ready = 0;
	memset(wb.usart3_arr, 0, sizeof(wb.usart3_arr));
	GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

void set_wifi_mode(WIFI_MODE mode){
	wifi_mode=mode;
}

void wifi_connect(void){
	set_wifi_mode(CONNECT);
}
void wifi_disconnect(void){
	set_wifi_mode(DISCONNECT);
}


