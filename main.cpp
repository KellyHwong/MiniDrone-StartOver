#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Misc/Formatter.h"
#include "Misc/Delay.h"
#include "Utilities/LCD.h"


// 持久性变量，全局
Timer tim5(5);

int main(void)
{
  APP_Init();
  uint8_t str_let[] = "Let the hunt begin.";
  GUI_Text(0,0,str_let,White,Blue);
  while (1)
  {

	}
  
  return -1;
}

void APP_Init(void) {
  // LCD
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOD , GPIO_Pin_7);    //CS=0;
  LCD_Initializtion();
  LCD_Clear(Blue);
  // timer5 as pwm input
  tim5.mode_pwm_input(PA1); // 一键设置为PWM输入
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
