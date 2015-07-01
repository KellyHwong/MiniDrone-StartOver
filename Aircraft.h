#ifndef MAIN_H_
#define MAIN_H_
#ifdef __cplusplus
 extern "C" {
#endif 
 
void Aircraft_Init(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);
   
#ifdef __cplusplus
}
#endif

#endif
