#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"
#include "LCD/LCD.h"
#include "TIM5_PWM_Cap.h"
#include "Formatter.h"



void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}



void lcd_display(void) {
  //int_to_string(CCR2_Val, str2);
  //int_to_string(CCR1_Val, str1);
  LCD_Clear(Red);
  GUI_Text(132,120,str2,White,Red);
  Delay(30000000); // 大概是0.5S
	
  LCD_Clear(Red);
  GUI_Text(76,120,str1,White,Red);
  Delay(30000000);	  
}

uint16_t test_color = 0;
void lcd_test(void) {
  float f = 123.4567; // test float_to_string
  uint8_t f_str[80];
  float_to_string(f,f_str);
  LCD_Clear(Blue);
  //GUI_Text(0,0,"cao ni ma bi de",White,Blue);
  //GUI_Text(0,LCD_LINE_SPACE,"ying jian ke she!!!",White,Blue);
  GUI_Text(0,0,f_str,White,Blue);
  Delay(30000000);
}

void APP_Init(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOD , GPIO_Pin_7);		 //CS=0;
	// LCD
  LCD_Initializtion();
  LCD_Clear(Blue);
  // PWM detect
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM5_CH1_Cap_Init(0xFFFFFFFF, 84-1);
  TIM5_CH1_Cap_Init(0xFFFFFFFF,84-1); // 1MHz计时频率，1uS计一次
}

extern uint8_t TIM5CH1_CAPTURE_STA;
extern uint32_t TIM5CH1_CAPTURE_VAL;
extern float DutyCycle;
extern float Frequency;

uint8_t str1[80];
uint8_t str2[80];
Timer tim1(TIM1);

int main(void)
{
  uint64_t tmp = 0;
  uint8_t str[80];
  APP_Init();
  
  GUI_Text(0,0,"Let the hunt begin.",White,Blue);
  while (1)
  {
    
	}
  return -1;
}

void APP_Init(void) {
  tim1.mode_pwm_input(); // 一键设置为PWM输入
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
