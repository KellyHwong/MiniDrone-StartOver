#ifndef DELAY_H_
#define DELAY_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

void delay_configuration(void);
void delay_cmd(FunctionalState state);
void delay_ms(vu32);
void delay_us(vu32);

#ifdef __cplusplus
}
#endif

#endif
