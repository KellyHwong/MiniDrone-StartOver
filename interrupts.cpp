/**  
* @Filename: interrupts.cpp
* @Author: Kelly Hwong
* @Update: 2015.6.30
* @Description: stm32中断接口
*/
#include "Drivers/Timer.h"
extern Timer tim5;

void TIM1_IRQHandler() {

}
void TIM2_IRQHandler() {

}
void TIM3_IRQHandler() {

}
void TIM4_IRQHandler() {

}
void TIM5_IRQHandler() {
  (*tim5.pFnIRQHandler)();
}
void TIM6_IRQHandler() {

}
void TIM7_IRQHandler() {

}
void TIM8_IRQHandler() {

}
