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
 *功能: 车辆模块： 新增或修改车辆信息
 *参数:
 *	@:car:车辆信息
 *返回值: 参考StatusCode枚举
 */
void Send_String(USART_TypeDef * USARTx,const char *str);

#endif

