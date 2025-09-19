#ifndef __WIFI_H__
#define __WIFI_H__
#include <stm32f4xx.h>
#include "usart.h"
#include <string.h>
#define SUCCESS 1
#define FAIL 0

void init_wifi_mode(void);

void send_wifi_command(const char *str);
void receve_wifi_response(void);


uint8_t wait_wifi_response(const char*str,uint32_t timeout);


#endif

