#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include <stdio.h>

extern u8 flag;

void usart_x_init(u32 BaudRate);
void USART3_SendData(unsigned char ch);
void SendData(signed int A, signed int B, signed int C, signed int D);
void MPU6050_NVIC_Configuration(void);

void usart_configuration(USART_TypeDef* usartx, uint32_t baudRate);
void usart_sendByte(USART_TypeDef *usart, u8 val);
void usart_sendBytes(USART_TypeDef *usart, u8 *buffer, u16 len);

#ifdef __cplusplus
}
#endif

#endif
