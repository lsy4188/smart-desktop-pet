
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
    wb.state = AT_RST;
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
		//send_string(USART6, (char*)wb.usart3_arr);
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



/*void wifi_connect_process(void){
	switch(wb.state){
		case WIFI_STATE_IDLE:
			if(receve_wifi_response("ready")){
				wb.state=WIFI_STATE_AT;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
				}else{
					wb.state=WIFI_ERROR;
				}	
			}
			break;
		case WIFI_STATE_AT:
			if(receve_wifi_response("OK")){
				wb.state=WIFI_STATE_RESTORE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+RESTORE");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT");
				}else{
					wb.state=WIFI_ERROR;
				}	
			}
			break;
			
		case WIFI_STATE_RESTORE:
			if(receve_wifi_response("OK")){
				wb.state=WIFI_STATE_CWMODE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWMODE=1");
			}else if(wb.timeout_counter>4000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+RESTORE");
				}else{
					wb.state=WIFI_ERROR;
				}
			}
			break;
			
		case WIFI_STATE_CWMODE: 
			if(receve_wifi_response("OK")){
				wb.state=WIFI_STATE_CWJAP;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
			}else if(wb.timeout_counter>6000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWMODE=1");
				}else{
					wb.state=WIFI_ERROR;
				}
			}
			break;
			
		case WIFI_STATE_CWJAP: 
			if (receve_wifi_response("OK")) {
                wb.state = CREATE_TCP_CONNECT;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
            } else if (wb.timeout_counter > 8000) {
                if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
				}else{
					wb.state=WIFI_ERROR;
				}
            }
			break;
		case CREATE_TCP_CONNECT: 
			if (receve_wifi_response("OK")) {
                wb.state = WIFI_DONE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				wb.is_connect=1;
            } else if (wb.timeout_counter > 9000) {
                if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
				}else{
					wb.state=WIFI_ERROR;
				}
            }
			break;
		case WIFI_DONE: 
			receve_wifi_response("OK");
			break;
			
		case WIFI_ERROR: 
			GPIO_SetBits(GPIOA, GPIO_Pin_15);
			break;		
	}
}*/

void wifi_connect_process(void){
	switch(wb.state){
		case AT_RST:
			if(receve_wifi_response("ready")){
				wb.state=WIFI_STATE_AT;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+RST");
				}else{
					wb.state=WIFI_ERROR;
				}	
			}
			break;
		case WIFI_STATE_AT:
			if(receve_wifi_response("OK")){
				wb.state=WIFI_STATE_CWMODE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWMODE=1");
			}else if(wb.timeout_counter>3000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT");
				}else{
					wb.state=WIFI_ERROR;
				}	
			}
			break;	
		case WIFI_STATE_CWMODE: 
			if(receve_wifi_response("OK")){
				wb.state=WIFI_STATE_CWJAP;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
			}else if(wb.timeout_counter>6000){
				if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWMODE=1");
				}else{
					wb.state=WIFI_ERROR;
				}
			}
			break;	
		case WIFI_STATE_CWJAP: 
			if (receve_wifi_response("OK")) {
                wb.state = CREATE_TCP_CONNECT;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
            } else if (wb.timeout_counter > 8000) {
                if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
				}else{
					wb.state=WIFI_ERROR;
				}
            }
			break;
		case CREATE_TCP_CONNECT: 
			if (receve_wifi_response("OK")) {
                wb.state = WIFI_DONE;
				wb.timeout_counter = 0;
				wb.retry_count=0;
				wb.is_connect=1;
            } else if (wb.timeout_counter > 9000) {
                if(wb.retry_count++<3){
					wb.timeout_counter = 0;
					send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
				}else{
					wb.state=WIFI_ERROR;
				}
            }
			break;
		case WIFI_DONE: 
			receve_wifi_response("OK");
			break;
			
		case WIFI_ERROR: 
			GPIO_SetBits(GPIOA, GPIO_Pin_15);
			break;		
	}
}


/*void wifi_disconnect_process(void){
	switch(wb.state){
			case WIFI_STATE_IDLE:
				if(receve_wifi_response("ready")){
					wb.state=WIFI_STATE_AT;
					wb.timeout_counter = 0;
					wb.retry_count=0;
					send_wifi_command("AT");
				}else if(wb.timeout_counter>3000){
					if(wb.retry_count++<3){
						wb.timeout_counter = 0;
					}else{
						wb.state=WIFI_ERROR;
					}	
				}
				break;
			case WIFI_STATE_AT:
				if(receve_wifi_response("OK")){
					wb.state=WIFI_STATE_RESTORE;
					wb.timeout_counter = 0;
					wb.retry_count=0;
					send_wifi_command("AT+RESTORE");
				}else if(wb.timeout_counter>3000){
					if(wb.retry_count++<3){
						wb.timeout_counter = 0;
						send_wifi_command("AT");
					}else{
						wb.state=WIFI_ERROR;
					}	
				}
				break;
				
			case WIFI_STATE_RESTORE:
				if(receve_wifi_response("OK")){
					wb.state=WIFI_STATE_CWMODE;
					wb.timeout_counter = 0;
					wb.retry_count=0;
					send_wifi_command("AT+CWMODE=1");
				}else if(wb.timeout_counter>4000){
					if(wb.retry_count++<3){
						wb.timeout_counter = 0;
						send_wifi_command("AT+RESTORE");
					}else{
						wb.state=WIFI_ERROR;
					}
				}
				break;
				
			case WIFI_STATE_CWMODE: 
				if(receve_wifi_response("OK")){
					wb.state=WIFI_STATE_CWJAP;
					wb.timeout_counter = 0;
					wb.retry_count=0;
					send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
				}else if(wb.timeout_counter>6000){
					if(wb.retry_count++<3){
						wb.timeout_counter = 0;
						send_wifi_command("AT+CWMODE=1");
					}else{
						wb.state=WIFI_ERROR;
					}
				}
				break;
				
			case WIFI_STATE_CWJAP: 
				if (receve_wifi_response("OK")) {
					wb.state = CREATE_TCP_CONNECT;
					wb.timeout_counter = 0;
					wb.retry_count=0;
					send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
				} else if (wb.timeout_counter > 8000) {
					if(wb.retry_count++<3){
						wb.timeout_counter = 0;
						send_wifi_command("AT+CWJAP_DEF=\"hqyjcs\",\"hqyj2022\"");
					}else{
						wb.state=WIFI_ERROR;
					}
				}
				break;
			case CREATE_TCP_CONNECT: 
				if (receve_wifi_response("OK")) {
					wb.state = WIFI_DONE;
					wb.timeout_counter = 0;
					wb.retry_count=0;
					wb.is_connect=1;
				} else if (wb.timeout_counter > 9000) {
					if(wb.retry_count++<3){
						wb.timeout_counter = 0;
						send_wifi_command("AT+CIPSTART=\"TCP\",\"192.168.20.52\",8080");
					}else{
						wb.state=WIFI_ERROR;
					}
				}
				break;
			case WIFI_DONE: 
				receve_wifi_response("OK");
				break;
				
			case WIFI_ERROR: 
				GPIO_SetBits(GPIOA, GPIO_Pin_15);
				break;		
		}


}*/


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

/******************以下函数用于语音控制*****************************/
void update_wifi_mode(void){
    static WIFI_MODE last_mode = DEFAULT;
    if (last_mode != wifi_mode) {
        last_mode = wifi_mode;
    }
    switch (wifi_mode) {
        case CONNECT:
            wifi_connect_process();
            break;
        case DISCONNECT:
			//wifi_disconnect_process();
            break;
		 case DEFAULT:
            break;
    }
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

void wifi_default(void){
	set_wifi_mode(DEFAULT);
}


