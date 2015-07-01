#include "Aircraft.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Misc/Formatter.h"
#include "Misc/Delay.h"
#include "Utilities/LCD.h"
//#include "Utilities/Motor.h"
//#include "Utilities/Receiver.h"
//#include "Utilities/Stick.h"

// 宏定义
// 信号接收定时器，引脚
#define THROTTLE_TIM 1
#define PITCH_TIM    2
#define YAW_TIM      3
#define ROLL_TIM     4
#define THROTTLE_PIN PA9
#define PITCH_PIN    PB3
#define YAW_PIN      PC7
#define ROLL_PIN     PD13

// 三个通道捕获计时器
Timer tim_throttle(THROTTLE_TIM); // ch3
Timer tim_pitch(PITCH_TIM); // ch1
Timer tim_yaw(YAW_TIM); // ch2
Timer tim_roll(ROLL_TIM);
/*
// Motors
Motor motor1(PWM(50,0.05,9,1));
Motor motor2(PWM(50,0.05,9,2));
Motor motor3(PWM(50,0.05,10,1));
Motor motor4(PWM(50,0.05,11,1));
// 调度器
Timer tim_sch();
// 通道捕获器和解释器
Receiver receiver; 
receiver.stick_throttle = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门
receiver.stick_pitch = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门
receiver.stick_roll = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门
receiver.stick_yaw = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门
*/
int main(void)
{
  Aircraft_Init();
  uint8_t str_let[] = "Let the hunt begin.";
  GUI_Text(0,0,str_let,White,Blue);
  while (1)
  {

	}
  
  return -1;
}

void Aircraft_Init(void) {
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
  // Capturers
  tim_throttle.mode_pwm_input(THROTTLE_PIN);
  tim_pitch.mode_pwm_input(PITCH_PIN);
  tim_yaw.mode_pwm_input(YAW_PIN);
  tim_roll.mode_pwm_input(ROLL_PIN);
}

void TIM1_IRQHandler(void) {
  tim_throttle.PWM_Input_Handler();
  uint8_t tmp2[80];
  float_to_string(tim_throttle.DutyCycle,tmp2);
  uint8_t tmp1[80] = "throttle:";
  GUI_Text(0,0,tmp1,White,Blue);
  GUI_Text(0,LCD_LINE_SPACE,tmp2,White,Blue);
}
void TIM2_IRQHandler(void) {
  tim_pitch.PWM_Input_Handler();
  uint8_t tmp2[80];
  float_to_string(tim_pitch.DutyCycle,tmp2);
  uint8_t tmp1[80] = "pitch:";
  GUI_Text(0,2*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,3*LCD_LINE_SPACE,tmp2,White,Blue);
}
void TIM3_IRQHandler(void) {
  tim_yaw.PWM_Input_Handler();
  uint8_t tmp2[80];
  float_to_string(tim_yaw.DutyCycle,tmp2);
  uint8_t tmp1[80] = "yaw:";
  GUI_Text(0,4*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,5*LCD_LINE_SPACE,tmp2,White,Blue);
}
void TIM4_IRQHandler(void) {
  tim_roll.PWM_Input_Handler();
  uint8_t tmp2[80];
  float_to_string(tim_roll.DutyCycle,tmp2);
  uint8_t tmp1[80] = "roll:";
  GUI_Text(0,6*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,7*LCD_LINE_SPACE,tmp2,White,Blue);
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
