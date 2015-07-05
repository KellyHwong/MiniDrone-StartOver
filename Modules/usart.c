#include "usart.h"
#include "util.h"



void usart_configuration(USART_TypeDef* usartx, uint32_t baudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_TypeDef* gpio_txd;
	GPIO_TypeDef* gpio_rxd;
	uint16_t pinSource_txd;
	uint16_t pinSource_rxd;
	uint8_t gpio_af;
	uint32_t pin_txd;
	uint32_t pin_rxd;
	uint8_t irq_channel;

	uint8_t pre_priority = 1;
	uint8_t sub_priority = 1;

  // PB6 和 PB7 的USART
	if(usartx == USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		gpio_txd = GPIOB;
		gpio_rxd = GPIOB;
		pinSource_txd = GPIO_PinSource6;
		pinSource_rxd = GPIO_PinSource7;
		gpio_af = GPIO_AF_USART1;
		pin_txd = GPIO_Pin_6;
		pin_rxd = GPIO_Pin_7;
		irq_channel = USART1_IRQn;
	}
	else if(usartx == USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		gpio_txd = GPIOA;
		gpio_rxd = GPIOA;
		pinSource_txd = GPIO_PinSource2;
		pinSource_rxd = GPIO_PinSource3;
		gpio_af = GPIO_AF_USART2;
		pin_txd = GPIO_Pin_2;
		pin_rxd = GPIO_Pin_3;
		irq_channel = USART2_IRQn;
	}
	else if(usartx == USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		gpio_txd = GPIOB;
		gpio_rxd = GPIOB;
		pinSource_txd = GPIO_PinSource10;
		pinSource_rxd = GPIO_PinSource11;
		gpio_af = GPIO_AF_USART3;
		pin_txd = GPIO_Pin_10;
		pin_rxd = GPIO_Pin_11;
		irq_channel = USART3_IRQn;
	}
	else if(usartx == UART4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		gpio_txd = GPIOC;
		gpio_rxd = GPIOC;
		pinSource_txd = GPIO_PinSource10;
		pinSource_rxd = GPIO_PinSource11;
		gpio_af = GPIO_AF_UART4;
		pin_txd = GPIO_Pin_10;
		pin_rxd = GPIO_Pin_11;
		irq_channel = UART4_IRQn;
	}
	else if(usartx == UART5){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		gpio_txd = GPIOC;
		gpio_rxd = GPIOD;
		pinSource_txd = GPIO_PinSource12;
		pinSource_rxd = GPIO_PinSource2;
		gpio_af = GPIO_AF_UART5;
		pin_txd = GPIO_Pin_12;
		pin_rxd = GPIO_Pin_2;
		irq_channel = UART5_IRQn;
	}
	else{
		while(1);
	}

	GPIO_PinAFConfig(gpio_txd, pinSource_txd, gpio_af);
    GPIO_PinAFConfig(gpio_rxd, pinSource_rxd, gpio_af);

	portOutInit(gpio_txd, pin_txd, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz);
	portOutInit(gpio_rxd, pin_rxd, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz);

	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(usartx,&USART_InitStructure);
	USART_ITConfig(usartx,USART_IT_RXNE,ENABLE);
	USART_Cmd(usartx,ENABLE);
	USART_ClearFlag(usartx, USART_FLAG_TC);

	nvic_configuration(irq_channel, pre_priority, sub_priority);
}

int fputc(int ch, FILE *f)
{
	USART_SendData(UART4, (uint8_t) ch);
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
	return ch;
}

void usart_sendByte(USART_TypeDef *usart, u8 val)
{
	USART_SendData(usart, val);
	while (USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET);
}

void usart_sendBytes(USART_TypeDef *usart, u8 *buffer, u16 len)
{
	u16 i;
	for(i=0; i<len; i++){
		usart_sendByte(usart, buffer[i]);
	}
}

extern float pitch, roll, yaw;

void get_euler_angle(u8 *bufdata)
{
	pitch =((bufdata[3] << 8) | bufdata[2]) / 32768.0f * 180.0f;
	if(pitch >= 270.0f){
		pitch -= 360;
	}
	pitch = -pitch;
	roll=((bufdata[5] << 8) | bufdata[4]) / 32768.0f * 180.0f;
	if(roll >= 270.0f){
		roll -= 360.0f;
	}
	yaw  =((bufdata[7] << 8) | bufdata[6]) / 32768.0f * 180.0f;
	if(yaw >= 180.0f){
		yaw -= 360.0f;
	}
}

u8 flag = 0;

void USART1_IRQHandler(void)
{
	static u8 bufdata[11];
	static u8 count = 0;
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
	{
		bufdata[count] = USART_ReceiveData(USART1);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);

		if(bufdata[0] != 0x55 && count == 0)
			return;
		count++;
		if(count == 2 && bufdata[1] != 0X53)
		{
			count = 0;
			return;
		}
		if(count == 11)
		{
			count = 0;
			get_euler_angle(bufdata);
			flag = 1;
		}
	}
}

void UART4_IRQHandler(void)
{
	if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE)==SET)
	{
		USART_ReceiveData(UART4);
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);

	}
}

#include "usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"

// 张哲负责的代码
void usart_x_init(u32 BaudRate)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);


  	USART_InitStructure.USART_BaudRate = BaudRate;					//?????
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  	USART_Init(USART3, &USART_InitStructure);

	  USART_ITConfig(USART3, USART_IT_TXE, DISABLE);  				//??????
    USART_ClearFlag(USART3,USART_FLAG_TC);
	  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);					//??????

	  USART_ITConfig(USART3, USART_IT_PE, ENABLE);    //??PE??????Bit 8PEIE: PE interrupt enable

  	USART_ITConfig(USART3, USART_IT_ERR, ENABLE);		//CR2 ??ERR??

  	USART_Cmd(USART3, ENABLE);	  									//??UART3
}

void USART3_SendData(unsigned char ch)
{

		USART_SendData(USART3, (u8) ch);
  		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

void SendData(signed int A, signed int B, signed int C, signed int D)
{
  uint8_t check_byte;
  USART3_SendData(0x55);
  USART3_SendData(0x34);

  USART3_SendData(0x08);                                 //???? 8 byte

	//if(A<0)A=32768-A;
  USART3_SendData(A>>8);
  USART3_SendData(A);

	//if(B<0)B=32768-B;
  USART3_SendData(B>>8);
  USART3_SendData(B);

	//if(C<0)C=32768-C;
  USART3_SendData(C>>8);
  USART3_SendData(C);

	//if(D<0)D=32768-D;
  USART3_SendData(D>>8);
  USART3_SendData(D);

  check_byte  = (A>>8)^A^(B>>8)^B^(C>>8)^C^(D>>8)^D;                            //???? ??????

  USART3_SendData(check_byte);
}

void MPU6050_NVIC_Configuration(void) {
  	NVIC_InitTypeDef NVIC_InitStructure;//Usart1 NVIC ??

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
   	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????
		NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART3
}


#if 1
#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};
FILE __stdout;
_sys_exit(int x)
{
	x = x;
}

// 这段程序好像有问题
/* int fputc(int ch, FILE *f)
{
  	USART_SendData(USART3, (u8) ch);
  	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
  	return ch;
} */

#endif

