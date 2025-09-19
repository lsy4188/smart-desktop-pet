#include "SHT20.h"

sht20 sh={0};
void init_SHT20(void){
	init_i2c_cust();
}

void Reset_SHT20(void){
	start_i2c_cust();
	write_data_cust(SHT20_I2C_ADDR<<1);
	if(wait_ACK_cust()==NACK){
		stop_i2c_cust();
		return ;
	}
	write_data_cust(0xFE);
	if(wait_ACK_cust()==NACK){
		stop_i2c_cust();
		return ;
	}
	stop_i2c_cust();
}

uint16_t read_data(uint8_t cmd){
	uint8_t data[2]={0};
	start_i2c_cust();
	write_data_cust(SHT20_I2C_ADDR<<1);
	if(wait_ACK_cust()==NACK){
		stop_i2c_cust();
		return NACK;
	}
	write_data_cust(cmd);
	if(wait_ACK_cust()==NACK){
		stop_i2c_cust();
		return NACK;
	}

	start_i2c_cust();
	write_data_cust(SHT20_I2C_ADDR<<1|0x01);
	if(wait_ACK_cust()==NACK){
		stop_i2c_cust();
		return NACK;
	}
	if(cmd==SHT20_T_HOLD_MASTER){
		delay_ms(85);
	}else if(cmd==SHT20_HR_HOLD_MASTER){
		delay_ms(29);
	}
	data[0]=read_data_cust(ACK);
	data[1]=read_data_cust(NACK);
	stop_i2c_cust();
	return data[0]<<8|data[1];
}

void Process_Data(char *str){
	float tem,rh;
	if(strcmp(str,"hold")==0){
		tem=read_data(SHT20_T_HOLD_MASTER);
		rh=read_data(SHT20_HR_HOLD_MASTER);
	}else{
		tem=read_data(SHT20_T_NHOLD_MASTER);
		rh=read_data(SHT20_HR_NHOLD_MASTER);
	}
	rh=-6+125*((double)rh/65536);
	tem=-46.85+175.72*((double)tem/65536);
	sh.tem=tem;
	sh.rh=rh;
	sh.flg=0;
	//printf("tem=%.2f<--------------->rh=%.2f\r\n",sh.tem,sh.rh);
	
}

/**********************以下是标准库实现的i2c***************************/
void init_SHT20V1(void){
	init_i2c();
	Reset_SHT20V1();

}

void Reset_SHT20V1(void){
	i2c_write(SHT20_I2C_ADDR<<1,0,0xFE);
}
uint16_t read_dataV1(uint16_t cmd){
	uint8_t data[2]={0};
	int i=0;
	for(i=0;i<2;i++){
		data[i]=i2c_read(SHT20_I2C_ADDR, cmd);
		//printf("%d\n",data[i]);
	}
	return data[0]<<8|data[1];
}
sht20 Process_DataV1(char *str){
	sht20 data;
		float tem,rh;
		if(strcmp(str,"hold")==0){
			tem=read_dataV1(SHT20_T_HOLD_MASTER);
			rh=read_dataV1(SHT20_HR_HOLD_MASTER);
		}else{
			tem=read_dataV1(SHT20_T_NHOLD_MASTER);
			rh=read_dataV1(SHT20_HR_NHOLD_MASTER);
		}
		rh=-6+125*((double)rh/65536);
		tem=-46.85+175.72*((double)tem/65536);
		data.tem=tem;
		data.rh=rh;
		return data;
}



