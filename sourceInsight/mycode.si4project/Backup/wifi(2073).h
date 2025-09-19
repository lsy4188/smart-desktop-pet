#ifndef __WIFI_H__
#define __WIFI_H__
#include <stm32f4xx.h>
#include "usart.h"
#include <string.h>
#define SUCCESS 1
#define FAIL 0

void init_wifi_mode(void);

void send_wifi_result(const char *str);

uint8_t receve_wifi_data(void);


#endif

