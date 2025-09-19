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
	CONNECT,
	DISCONNECT
}WIFI_MODE;

extern WIFI_MODE wifi_mode;

typedef enum {
	WIFI_STATE_IDLE=0,
    WIFI_STATE_AT=1,
    WIFI_STATE_RESTORE=2,
    WIFI_STATE_CWMODE=3,
    WIFI_STATE_CWJAP=4,
    CREATE_TCP_CONNECT=5,
    WIFI_ERROR=6,
    WIFI_DONE=7
} wifi_state_t;

typedef struct {
	uint8_t usart3_arr[MAX_SIZE];
	uint16_t index;
	uint8_t is_ready;
	wifi_state_t state;         
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
void wifi_process_state(void);



/******************以下函数用于语音控制*****************************/
void update_wifi_mode(void);
void set_wifi_mode(WIFI_MODE mode);
void wifi_connect(void);
void wifi_disconnect(void);



#endif

