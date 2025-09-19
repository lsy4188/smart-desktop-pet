/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
#include "key.h"
#include "tim.h"
#include "M74HC595.h"
#include "SHT20.h"
#include "usart.h"
#include "wifi.h"
#include "ASRPRO.h"
#include "spi.h"



/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  
void SVC_Handler(void)
{
}
*/

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
 
void PendSV_Handler(void)
{
} 
*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  
void SysTick_Handler(void)
{
}
*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
extern u8 mode;
extern u8 key_flag;
void EXTI9_5_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line5)){
		EXTI_ClearITPendingBit(EXTI_Line5);
		mode=LED;
		key_flag^=0x01;
	}
	if(EXTI_GetITStatus(EXTI_Line8)){
		EXTI_ClearITPendingBit(EXTI_Line8);
		mode=BEEP;	
		key_flag^=0x02;
	}else if(EXTI_GetITStatus(EXTI_Line9)){
		EXTI_ClearITPendingBit(EXTI_Line9);
		mode=FAN;
		key_flag^=0x04;	
	}
}

extern u8 cache[32];
extern u8 cmd_ready;
extern u8 cmd_index;
void USART6_IRQHandler(void){
	u16 data=0;
	if(USART_GetITStatus(USART6,USART_IT_RXNE)){
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
		data=USART_ReceiveData(USART6);
       if (cmd_index < 31) {
            cache[cmd_index++] = data;
        } else {
            cmd_index = 0;
        }
	}
	if(USART_GetITStatus(USART6,USART_IT_IDLE)){
         USART6->DR;//清零
		 if (cmd_index > 0) {
                cmd_ready = 1;
        }
	}
}

extern wifi_buffer wb;
void USART3_IRQHandler(void){
	uint8_t data=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE)){
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		data=USART_ReceiveData(USART3);
		USART_SendData(USART6, data);
		
		if(wb.index<MAX_SIZE-1){
			wb.usart3_arr[wb.index++]=data;
			//printf("wb.index: %d\n", wb.index); 
		}else{		
			//printf("wb.index full.........\n"); 
			wb.index=0;
			memset(wb.usart3_arr, 0, sizeof(wb.usart3_arr));
		}
		
	}
	if(strstr((char*)wb.usart3_arr,"ready")||strstr((char*)wb.usart3_arr,"OK")){
		if(wb.index>0){
			wb.is_ready=1;
			wb.usart3_arr[wb.index]='\0';
		}
	}
/*	//此处使用IDLE中断要检测数组中的是否有成功标志位
	if(USART_GetITStatus(USART3, USART_IT_IDLE)){
		//IDLE清零
		USART3->DR;
		if(wb.index>0){
			wb.is_ready=1;
		}
	}*/

}

extern ASRPRO asr;
void USART2_IRQHandler(void){
	uint8_t data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE)){
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		data=USART_ReceiveData(USART2);
		USART_SendData(USART6, data);
		if(asr.asr_index<ASR_MAX_SIZE-1){
			asr.asr_data[asr.asr_index++]=data;
		}else{
			asr.asr_index=0;
		}
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE)){
		USART2->DR;//清零
		 if (asr.asr_index > 0) {
           asr.asr_isready = 1;
		   asr.asr_data[asr.asr_index]='\0';
        }
	}	
}

void TIM7_IRQHandler(void){
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
		 if ((wb.connect_state != WIFI_DONE && wb.connect_state != WIFI_ERROR)||(wb.disconnect_state != DISCONNECT_DONE && wb.disconnect_state != DISCONNECT_WIFI_ERROR)) {
			wb.timeout_counter++;
        }
	}
}


extern sht20 sh;
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		sh.flg=1;
	}
}
extern uint8_t sht20_data[4];
extern uint8_t pos[];
uint8_t sht20_index=0;
void TIM4_IRQHandler(void){
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)){
			SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);//拉低NSS开始通讯
			SPI_I2S_SendData(SPI2, pos[sht20_index]);
			while (!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
			SPI_I2S_SendData(SPI2, sht20_data[sht20_index]);
			while (!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
			GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高NSS结束通讯
			sht20_index = (sht20_index + 1) % 4;
		}
	}
}


void I2C1_EV_IRQHandler(){
	if(I2C_GetITStatus(I2C1, I2C_IT_RXNE)){
		I2C_ClearITPendingBit(I2C1, I2C_IT_RXNE);
		USART_SendData(USART6,I2C_ReceiveData(I2C1));
	}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
