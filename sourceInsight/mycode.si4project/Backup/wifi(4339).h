#ifndef __WIFI_H__
#define __WIFI_H__
#include <stm32f4xx.h>
#include "usart.h"
#include "tim.h"
#include <string.h>
#define SUCCESS 1
#define FAIL 0

void init_wifi_mode(void);

void send_wifi_command(char *str);

void receve_wifi_response(void);
uint8_t receve_wifi_responseV1(const char* expected, uint16_t timeout);



#endif

