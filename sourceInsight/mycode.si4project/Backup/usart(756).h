#ifndef __USART_H__
#define __USART_H__
#include <stm32f4xx.h>

/*
 *功能: 串口初始化
 */
void init_usart(void);
/*
 *功能: 串口命令解析
 */
void parse_command(void);

/*
 *功能: 发送字符串
 *参数:
 *	@:USARTx:USART外设
 *	@:str:字符串
 */
void send_string(USART_TypeDef * USARTx,const char *str);

#endif

