#include "stm32f4xx.h"
#include "util.h"
#include "usart.h"
#include "delay.h"

float pitch, roll, yaw;

int main()
{
	nvic_grouping(2);
	rcc_configuration();
	delay_configuration();
	usart_configuration(UART4, 115200);
	usart_configuration(USART1, 115200);
	
	printf("RCC_Clocks.SYSCLK_Frequency = %d\r\n", rcc_clocks.SYSCLK_Frequency);
	printf("RCC_Clocks.HCLK_Frequency = %d\r\n", rcc_clocks.HCLK_Frequency);
	printf("RCC_Clocks.PCLK1_Frequency = %d\r\n", rcc_clocks.PCLK1_Frequency);
	printf("RCC_Clocks.PCLK2_Frequency = %d\r\n", rcc_clocks.PCLK2_Frequency);
	
	while(1){
		if(flag == 1){
			flag = 0;
			printf("pitch=%6.3f, roll=%6.3f, yaw=%6.3f\r\n", pitch, roll, yaw);
		}
	}
}

