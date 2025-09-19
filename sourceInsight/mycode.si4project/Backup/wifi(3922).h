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
#define MAX_SIZE 512

typedef struct {
	uint8_t usart3_arr[MAX_SIZE];
	volatile uint16_t index;
	volatile uint8_t is_ready;
}wifi_buffer;

void init_wifi_mode(void);
void send_wifi_command(char *str);
uint8_t receve_wifi_response(const char* expected);
void control_device(char* device_id, char* state);
void create_tcp_connect(const char*server,uint16_t port, const char* data);
void destory_tcp_connect(void);





#endif

