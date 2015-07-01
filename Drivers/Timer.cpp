#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Timer.h"
// assert(系统时钟为168MHz)
#define PRESCALER (84-1) //
// 挂载在APB1上，APB1默认分频4
// TIM2~7输入时钟为SYS/APB1*2（APB1!=1时）
// 系统时钟168MHz时，TIM2~7时钟频率84MHz（168/4*2）
// 计数预分频84，则计数频率1MHz，1uS计一次

#define PERIOD (0xFFFF) // 16位，兼容各个计时器

Timer::Timer(TIM_TypeDef * TIM, Timer_ModeTypedef Timer_Mode) {
  this.TIM = TIM;
  this.Timer_Mode = Timer_Mode;
  this.config_IRQn();
}

// 计时器设置为PWM捕获器，此时只能捕获1路
void Timer::mode_pwm_input() {
  this.config_Periph();
  // TIM5, CH2, 捕获PA1
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // PORTA 时能

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // 下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5); // PA0复用TIM5

  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ 通道使能
  NVIC_Init(&NVIC_InitStructure);

  // 时钟预分频，官方代码没有这一段
  TIM_TimeBaseStructrue.TIM_Prescaler = psc;
  TIM_TimeBaseStructrue.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructrue.TIM_Period = arr;
  TIM_TimeBaseStructrue.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructrue);

  // 设置TIM5 CH2 pin为外部信号输入
  // CCR2测频率
  // CCR1测占空比
  TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; // input channel 2
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿中断
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // map IC1 to TI1
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // 输入分频，不分频
  TIM5_ICInitStructure.TIM_ICFilter = 0x00; //滤波设置，经历几个周期跳变认定波形稳定0x0～0xF

  TIM_PWMIConfig(TIM5, &TIM5_ICInitStructure);

  /* Select the TIM5 Input Trigger: TI2FP2 */
  TIM_SelectInputTrigger(TIM5, TIM_TS_TI2FP2);

  TIM_SelectSlaveMode(TIM5,TIM_SlaveMode_Reset);//TIM从模式：触发信号的上升沿重新初始化计数器和触发寄存器的更新事件
  TIM_SelectMasterSlaveMode(TIM5,TIM_MasterSlaveMode_Enable); //启动定时器的被动触发

  TIM_Cmd(TIM5, ENABLE); // run TIM5
  TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE); // 打开中断，TIM_IT_Update不要
}

uint32_t CCR2_Val;
float DutyCycle = 0;
float Frequency = 0;

void TIM5_IRQHandler(void) {
  uint8_t duty_str[80];
  uint8_t freq_str[80];
  TIM_ClearITPendingBit(TIM5, TIM_IT_CC2); //
  CCR2_Val = TIM_GetCapture2(TIM5);
  if (CCR2_Val!=0) {
    DutyCycle= (TIM_GetCapture1(TIM5) * 100.0) / CCR2_Val;
    Frequency= 1000000.0 / CCR2_Val;
    float_to_string(DutyCycle, duty_str);
    float_to_string(Frequency, freq_str);
    //LCD_Clear(Blue);
    GUI_Text(0,LCD_LINE_SPACE,duty_str,White,Blue);
    GUI_Text(0,2*LCD_LINE_SPACE,freq_str,White,Blue);
  }
  else {
    DutyCycle= 0;
    Frequency= 0;
  }
}
}

// 配置GPIO端口
void Timer::config_pwm_input() {
  switch (this.TIM) {
    case (TIM1):
  }
}

void Timer::alternate_pwm_input_pin() {

}

// 配置IRQ通道
void Timer::config_IRQn() {
  switch (this.TIM) {
    case (TIM1): {this.IRQn = TIM1_IRQn; break;}
    case (TIM2): {this.IRQn = TIM2_IRQn; break;}
    case (TIM3): {this.IRQn = TIM3_IRQn; break;}
    case (TIM4): {this.IRQn = TIM4_IRQn; break;}
    case (TIM5): {this.IRQn = TIM5_IRQn; break;}
    case (TIM6): {this.IRQn = TIM6_IRQn; break;}
    case (TIM7): {this.IRQn = TIM7_IRQn; break;}
    case (TIM8): {this.IRQn = TIM8_IRQn; break;}
  }
}

// 配置外设
void Timer::config_Periph() {
  switch (this.TIM) {
    case (TIM1): { this.RCC_Periph_TIM = RCC_APB2Periph_TIM1; break; }
    case (TIM2): { this.RCC_Periph_TIM = RCC_APB1Periph_TIM2; break; }
    case (TIM3): { this.RCC_Periph_TIM = RCC_APB1Periph_TIM3; break; }
    case (TIM4): { this.RCC_Periph_TIM = RCC_APB1Periph_TIM4; break; }
    case (TIM5): { this.RCC_Periph_TIM = RCC_APB1Periph_TIM5; break; }
    case (TIM6): { this.RCC_Periph_TIM = RCC_APB1Periph_TIM6; break; }
    case (TIM7): { this.RCC_Periph_TIM = RCC_APB1Periph_TIM7; break; }
    case (TIM8): { this.RCC_Periph_TIM = RCC_APB2Periph_TIM8; break; }
  }
  RCC_APB1PeriphClockCmd(this.RCC_APB1Periph_TIM5, ENABLE); // TIM5 使能
}

// 计时器设置为PWM发生器，可以四路输出
void Timer::mode_pwm_output() {

}


