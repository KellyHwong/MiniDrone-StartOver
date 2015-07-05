#ifndef __UTIL_H__
#define __UTIL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f4xx.h"

#define max(a, b)			(a>b? a:b)
#define min(a, b)			(a<b? a:b)
#define range(x, a, b)		(min(max(x, a), b))
#define exchange(a, b, tmp) (tmp=a, a=b, b=tmp)

extern RCC_ClocksTypeDef rcc_clocks;

void rcc_configuration(void);
void nvic_grouping(u8 group_num);
void nvic_configuration(uint8_t irq_channel, uint8_t preemption_priority, uint8_t sub_priority);
void portOutInit(GPIO_TypeDef *gpio, uint32_t gpio_pin, GPIOMode_TypeDef mode, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pull_type, GPIOSpeed_TypeDef speed_type);
void portInInit(GPIO_TypeDef *gpio, uint32_t gpio_pin, GPIOMode_TypeDef mode, GPIOPuPd_TypeDef pull_type);
#ifdef __cplusplus
}
#endif
#endif
