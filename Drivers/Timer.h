/**
* @Filename:
* @Author: Kelly Hwong
* @Update: 2015.7.1
* @Description: stm32定时器类
*/
#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "Pin.h"

typedef enum {
  MODE_PWM_INPUT;
  MODE_PWM_OUTPUT;
} Timer_ModeTypedef;

class Timer {
 private:
  TIM_TypeDef * TIM; // eg: TIM5
  Timer_ModeTypedef Timer_Mode;
  // RCC
  uint32_t RCC_Periph_TIM;
  GPIO_InitTypeDef GPIO_InitStructure; // GPIO初始化器
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructrue; // 计时器分频初始化器
  TIM_ICInitTypeDef TIM_ICInitStructure; // 计时器模块初始化器
  NVIC_InitTypeDef NVIC_InitStructure;  // 中断初始化器

  uint32_t RCC_Periph_TIM;
  uint32_t RCC_Periph_GPIO;
  // GPIO
  Pin pin();
  // NVIC
  IRQn; // eg: TIM5_IRQn
  // TIM_ICInitStructure
  // uint16_t TIM_Channel; //eg: TIM_Channel_2
 public:
  // eg: Timer(TIM5, MODE_PWM_INPUT);
  Timer(TIM_TypeDef * TIM, Timer_ModeTypedef Timer_Mode);
}; // class Timer

#endif
