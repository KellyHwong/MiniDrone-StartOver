#ifndef TIM5_PWM_CAP_H_
#define TIM5_PWM_CAP_H_

#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "TIM5_PWM_Cap.h"

void TIM5_CH1_Cap_Init(uint32_t arr, uint16_t psc);

#endif
