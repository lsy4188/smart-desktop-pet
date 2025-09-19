#include "ASRPRO.h"

ASRPRO asr={0};
void init_ASRPRO(void){
	/*init_usart2();//配置放到了FreeRtos启动任务中了
	init_usart6();*/
	init_led();
	init_beep();
	init_fan();
	init_M74HC595();
	init_wifi_mode();
	memset(asr.asr_data,0,ASR_MAX_SIZE);
	asr.asr_index=0;
	asr.asr_isready=0;
}

void deal_with_asr(void){
	char* str=NULL;
	uint8_t i=0;
	if(asr.asr_isready){
		str=(char*)asr.asr_data;
		if(strstr(str,"dog1")||strstr(str,"dog2")||strstr(str,"dog3")){
			init_sg90(1);
		}else if(strstr(str,"dog4")){
			init_sg90(0);
		}
		for(i=0;i<DEV_COUNT;i++){
			if(strstr(str,devOpts[i].dev_name)){
				if(strstr(str,"open")){
					devOpts[i].open_fun();
				}else{
					devOpts[i].close_fun();
				}
				break;
			}
		}
		memset(asr.asr_data,0,sizeof(asr.asr_data));
		asr.asr_index=0;
		asr.asr_isready=0;
	}
}



