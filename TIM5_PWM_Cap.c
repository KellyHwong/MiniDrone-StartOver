#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "TIM5_PWM_Cap.h"
#include "Utilities/LCD.h"
#include "Formatter.h"

// TIM5, CH2, 捕获PA1
void TIM5_CH1_Cap_Init(uint32_t arr, uint16_t psc) {
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructrue;
  TIM_ICInitTypeDef TIM5_ICInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); // TIM5 使能
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // PORTA 时能

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // 下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5); // PA1复用TIM5

  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
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
