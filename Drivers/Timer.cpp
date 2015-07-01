#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"

// assert(系统时钟为168MHz)
#define PRESCALER (84-1) //
// 挂载在APB1上，APB1默认分频4
// TIM2~7输入时钟为SYS/APB1*2（APB1!=1时）
// 系统时钟168MHz时，TIM2~7时钟频率84MHz（168/4*2）
// 计数预分频84，则计数频率1MHz，1uS计一次

#define PERIOD (0xFFFFFFFF) // 32/16位，兼容各个计时器

Timer::Timer(uint8_t TIM_No) {
  this->TIM_No = TIM_No;
  switch (TIM_No) {
    case (1): { this->TIM = TIM1; break; }
    case (2): { this->TIM = TIM2; break; }
    case (3): { this->TIM = TIM3; break; }
    case (4): { this->TIM = TIM4; break; }
    case (5): { this->TIM = TIM5; break; }
    case (6): { this->TIM = TIM6; break; }
    case (7): { this->TIM = TIM7; break; }
    case (8): { this->TIM = TIM8; break; }
  };
  this->config_IRQn();
  this->config_Periph();
  //this->pFnIRQHandler = 0;
  this->Frequency = 0;
  this->DutyCycle = 0;
}

// 计时器设置为PWM捕获器，此时只能捕获1路
// eg: TIM5, CH2, 捕获PA1
void Timer::mode_pwm_input(PinTypedef p) {
  Pin pin(p); // 函数调用后变量就消失了
  uint8_t tmp = this->GPIO_AF_TIM(this->TIM);
  // 看看tmp有没有问题
  pin.mode_pwm_input(tmp);

  NVIC_InitTypeDef NVIC_InitStructure;  // 中断初始化器
  NVIC_InitStructure.NVIC_IRQChannel = this->IRQn;
  // 低频高优先级原则，20mS的捕获中断设为最高优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ 通道使能
  NVIC_Init(&NVIC_InitStructure);

  // 时钟预分频
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructrue; // 计时器分频初始化器
  TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER;
  TIM_TimeBaseStructrue.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructrue.TIM_Period = PERIOD;
  TIM_TimeBaseStructrue.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(this->TIM, &TIM_TimeBaseStructrue);

  // 设置TIM5 CH2 pin为外部信号输入
  // CCR2测频率
  // CCR1测占空比
  TIM_ICInitTypeDef TIM_ICInitStructure; // 计时器模块初始化器
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; // input channel 2
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿中断
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // map IC1 to TI1
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // 输入分频，不分频
  TIM_ICInitStructure.TIM_ICFilter = 0x00; //滤波设置，经历几个周期跳变认定波形稳定0x0～0xF

  TIM_PWMIConfig(this->TIM, &TIM_ICInitStructure);

  /* Select the TIM5 Input Trigger: TI2FP2 */
  TIM_SelectInputTrigger(this->TIM, TIM_TS_TI2FP2);

  TIM_SelectSlaveMode(this->TIM,TIM_SlaveMode_Reset);//TIM从模式：触发信号的上升沿重新初始化计数器和触发寄存器的更新事件
  TIM_SelectMasterSlaveMode(this->TIM,TIM_MasterSlaveMode_Enable); //启动定时器的被动触发

  TIM_Cmd(this->TIM, ENABLE); // run TIM5
  // 设置中断ISR为PWM Input模式中断
  // this->set_IRQHandler(Timer::PWM_Input_Handler_Dispatch);
  TIM_ITConfig(this->TIM, TIM_IT_CC2, ENABLE); // 打开中断，TIM_IT_Update不要
}

// 计算引脚复用地址
uint8_t Timer::GPIO_AF_TIM(TIM_TypeDef * TIM) {
  switch (this->TIM_No) {
    case (1): return GPIO_AF_TIM1;
    case (2): return GPIO_AF_TIM2;
    case (3): return GPIO_AF_TIM3;
    case (4): return GPIO_AF_TIM4;
    case (5): return GPIO_AF_TIM5;
    case (8): return GPIO_AF_TIM8;
    case (9): return GPIO_AF_TIM9;
    case (10): return GPIO_AF_TIM10;
    case (11): return GPIO_AF_TIM11;
    default: return 0xFF; // 错误输入，待处理
  }
}

// 配置IRQ通道
void Timer::config_IRQn(void) {
  switch (this->TIM_No) {
    // Warning!!! TIM1/6/8有多个IRQn，或者其IRQn不为TIMx_IRQn
    case (2): {this->IRQn = TIM2_IRQn; break;}
    case (3): {this->IRQn = TIM3_IRQn; break;}
    case (4): {this->IRQn = TIM4_IRQn; break;}
    case (5): {this->IRQn = TIM5_IRQn; break;}
    
    case (7): {this->IRQn = TIM7_IRQn; break;}
  }
}

// 配置外设
void Timer::config_Periph(void) {
  switch (this->TIM_No) {
    case (1): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM1; break; }
    case (2): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM2; break; }
    case (3): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM3; break; }
    case (4): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM4; break; }
    case (5): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM5; break; }
    case (6): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM6; break; }
    case (7): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM7; break; }
    case (8): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM8; break; }
  }
  switch (this->TIM_No) {
    // TIM2~7在APB1上
    case (2):
    case (3):
    case (4):
    case (5):
    case (6):
    case (7): {
      RCC_APB1PeriphClockCmd(this->RCC_Periph_TIM, ENABLE); // TIM 使能
      break;
    }
    // TIM1/8在APB2上
    case (1):
    case (8): {
      RCC_APB2PeriphClockCmd(this->RCC_Periph_TIM, ENABLE); // TIM 使能
      break;
    }
  }
}

// 计时器设置为PWM发生器，可以四路输出
void Timer::mode_pwm_output(void) {

}

/* void Timer::set_IRQHandler(void (Timer::*pFunction)(Timer * tim)) {
  pFnIRQHandler = pFunction;
} */

// PWM Input模式的中断处理函数
void Timer::PWM_Input_Handler(void) {
  uint32_t IC2Value;
  TIM_ClearITPendingBit(TIM, TIM_IT_CC2); //
  IC2Value = TIM_GetCapture2(TIM);
  if (IC2Value!=0) {
    DutyCycle= (TIM_GetCapture1(TIM) * 100.0) / IC2Value;
    Frequency= 1000000.0 / IC2Value;   
  }
  else {
    DutyCycle= 0;
    Frequency= 0;
  }
}

/* void Timer::PWM_Input_Handler_Dispatch(Timer * tim) {
  tim->PWM_Input_Handler();
} */
