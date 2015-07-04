#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#ifdef __cplusplus
 extern "C" {
#endif
// define configs，宏定义设置

//#define USING_CONTROLLER PID控制层写好后反注释这个

#define _DEBUG_MPU6050 // 调试MPU6050
//#define _DEBUG_MOTOR // 调试电机
//#define _DEBUG_USART // 调试USART
//#define _DEBUG_WITH_LCD // 在调试的时候使用LCD进行打印
//#define _DEBUG_STICK // 调试遥控器的杆子（测PWM范围的时候用的）

//#define INIT_MOTOR // 就是初始化电机，等待几秒的那些东西，试飞的时候需要反注释这个


#define DELAY_1S 40000000
#define PWM_FREQ 50
// 宏定义
// 信号接收定时器，引脚
#define THROTTLE_TIM 2
#define PITCH_TIM    3
#define YAW_TIM      4
#define ROLL_TIM     5
#define SCH_TIM      8
#define MOTOR1_PWM_TIM 9
#define MOTOR2_PWM_TIM 9
#define MOTOR3_PWM_TIM 11
#define MOTOR4_PWM_TIM 10
#define THROTTLE_PIN PB3
#define PITCH_PIN    PC7
#define YAW_PIN      PD13
#define ROLL_PIN     PA1
#define MOTOR1_PWM_CH  1
#define MOTOR1_PWM_PIN  PA2
#define MOTOR2_PWM_CH  2
#define MOTOR2_PWM_PIN  PA3
#define MOTOR3_PWM_CH  1
#define MOTOR3_PWM_PIN  PB9
#define MOTOR4_PWM_CH  1
#define MOTOR4_PWM_PIN  PB8

#define RXBUFFLE_SIZE 200

/* 这个宏定义很多文件都用到了，不要随便改！！！ */
#define SCHEDULER_TICK 0.0005 // 调度器时间间隔，单位S

void Aircraft_Init(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);
void TIM8_UP_TIM13_IRQHandler(void);
void USART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
