#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#ifdef __cplusplus
 extern "C" {
#endif
// define configs，宏定义设置

//#define USING_CONTROLLER PID控制层写好后反注释这个

//#define _DEBUG_MPU6050 // 调试MPU6050

// 调试电机和初始化电机要一起注释/反注释

//#define _DEBUG_MOTOR // 调试电机

//#define _DEBUG_USART // 调试USART
//#define _DEBUG_WITH_LCD // 在调试的时候使用LCD进行打印
//#define _DEBUG_STICK // 调试遥控器的杆子（测PWM范围的时候用的）


/* 应用层和控制层宏定义 */
#define RECEIVE_TICK 30 // 30，就是15mS一次
#define CONTROL_FREQ 114// 控制器调度频率，单位Hz
// PID参数
#define PID_ROLL_KP 3.3
#define PID_ROLL_KI 0.0
#define PID_ROLL_KD 1.0

#define PID_PITCH_KP 3.3
#define PID_PITCH_KI 0.0
#define PID_PITCH_KD 1.0
/* DEBUG 没有做yaw的 */
#define PID_YAW_KP 0.0
#define PID_YAW_KI 0.0
#define PID_YAW_KD 0.0
// 积分限幅，0.025/500*200 == 0.01
#define MAX_I_ABS 0.01


/* 底层宏定义 */
#define DELAY_1S 40000000
#define PWM_FREQ 50
// 信号接收定时器，引脚
#define THROTTLE_TIM 2
#define PITCH_TIM    3
#define YAW_TIM      4
#define ROLL_TIM     5
#define SCH_TIM      8
#define THROTTLE_PIN PB3
#define PITCH_PIN    PC7
#define YAW_PIN      PD13
#define ROLL_PIN     PA1
/*
四个电机的设置
2     1
  \ /
   X
  / \
3     4
*/
#define MOTOR1_PWM_TIM 9
#define MOTOR2_PWM_TIM 9
#define MOTOR3_PWM_TIM 11
#define MOTOR4_PWM_TIM 10
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
void Controller_Init(void);
void Receiver_Init(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);
void TIM8_UP_TIM13_IRQHandler(void);
void USART3_IRQHandler(void);
void USART_Routine(void);

#ifdef __cplusplus
}
#endif

#endif
