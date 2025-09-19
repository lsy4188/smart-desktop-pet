#ifndef __WIFI_H__
#define __WIFI_H__
#include <stm32f4xx.h>
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "fan.h"
#include "tim.h"
#include <string.h>
#define SUCCESS 1
#define FAIL 0
#define MAX_SIZE 100//512,70(正常),100(正常)

typedef enum{
	DEFAULT,
	CONNECT,
	DISCONNECT
}WIFI_MODE;

extern WIFI_MODE wifi_mode;
//连wifi流程集合
typedef enum {
	AT_RST				=0,
    WIFI_STATE_AT		=1,
    WIFI_STATE_CWMODE	=2,
    WIFI_STATE_CWJAP	=3,
    CREATE_TCP_CONNECT	=4,
    WIFI_ERROR			=5,
    WIFI_DONE			=6
} wifi_connect_state;

//断开WiFi流程集合
typedef enum {
    DESTORY_TCP_CONNECT		=0,
    DISCONNECT_WIFI			=1,
    WIFI_RESTORE			=2,
    DISCONNECT_WIFI_ERROR	=3,
    DISCONNECT_DONE			=4
} wifi_disconnect_state;


typedef struct {
	uint8_t usart3_arr[MAX_SIZE];
	uint16_t index;
	uint8_t is_ready;
	wifi_connect_state connect_state; 
	wifi_disconnect_state disconnect_state; 
    uint32_t timeout_counter;   
	uint8_t retry_count;
	uint8_t is_connect;
}wifi_buffer;

/*
此模块返回的数据之间会有延迟，会产生空闲中断
*/
void init_wifi_mode(void);
void send_wifi_command(char *str);
uint8_t receve_wifi_response(const char* expected);
void control_device(char* device_id, char* state);
void wifi_connect_process(void);
void wifi_disconnect_process(void);
void connect_error(void);
void disconnect_error(void);




/******************以下函数用于语音控制*****************************/
void update_wifi_mode(void);
void set_wifi_mode(WIFI_MODE mode);
void wifi_connect(void);
void wifi_disconnect(void);
void reset_wifi_state(void); 






#endif

