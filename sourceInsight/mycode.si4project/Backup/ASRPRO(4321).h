#ifndef __ASRPRO_H__
#define __ASRPRO_H__
#include <stm32f4xx.h>
#include <string.h>
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "fan.h"
#include "m74hc595.h"
#include "sg90.h"
#include "wifi.h"

#define ASR_MAX_SIZE 256
#define DEV_COUNT (sizeof(devOpts) / sizeof(devOpts[0]))
typedef void(*function)(void);
typedef struct {
	const char* dev_name;
	function open_fun;
	function close_fun;
}Dev_Opt;

//设备列表
static const Dev_Opt devOpts[]={
{"led",turn_on_all_led,turn_off_all_led},
{"beep",beep_open,beep_close},
{"fan",fan_on,fan_off},
{"tem",display_tem,display_close},
{"rh",display_rh,display_close},
{"wifi",wifi_connect,wifi_disconnect},
{"dog1",stand_action,forward_action},
{"dog2",turn_left_action,turn_right_action},
{"dog3",seat_squat_action,NULL},
{"dog4",back_action,NULL}
};


typedef struct{
	uint8_t asr_data[ASR_MAX_SIZE];
	uint16_t asr_index;
	uint8_t asr_isready;
}ASRPRO;

extern ASRPRO asr;
void init_ASRPRO(void);
void deal_with_asr(void);
#endif

