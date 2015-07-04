#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4_discovery.h"

#include "Aircraft.h"
#include "Delay.h"
#include "Formatter.h"
#include "LCD.h"
#include "Motor.h"
#include "MPU6050.h"
#include "Receiver.h"
#include "Stick.h"
#include "Timer.h"
#include "usart.h"

/* 全局变量声明 */
// 4个通道捕获计时器
Timer tim_throttle(THROTTLE_TIM); // 油门（升降） ch3
Timer tim_pitch(PITCH_TIM); // 俯仰（前后） ch2
Timer tim_yaw(YAW_TIM); // 偏航(水平转动) ch4
Timer tim_roll(ROLL_TIM); // 翻滚（左右） ch1
// 4个电机
Motor motor2;
Motor motor1;
Motor motor3;
Motor motor4;
// 调度器
Timer tim_sch(SCH_TIM);
// 通道捕获器和解释器
Receiver receiver;
// PID控制器
/* TODO
Controller controller;
*/
// 电机初始化
uint8_t motor_init_flag = 0;
// 接收缓冲区

uint8_t RxBuffer[RXBUFFLE_SIZE];
uint8_t firstinputflag;
uint8_t RxIndex;

int main(void)
{
  Aircraft_Init();
  while (1)
  {
#ifdef _DEBUG_MPU6050
    MPU6050_TEST();
#endif
#ifdef _DEBUG_MOTOR
    Delay(DELAY_1S/2);
    printf("%f   ",receiver.throttle_.convert_duty_);
#endif
#ifdef _DEBUG_STICK
    Delay(DELAY_1S/2);
    printf("%f   ",tim_yaw.DutyCycle);
#endif

    Delay(DELAY_1S/2);
    printf("%s",RxBuffer);
	}
  return -1;
}

void Aircraft_Init(void) {
  /* 控制层初始化，方便看所以写在前面 */
  // 接收机
  // 数据在2015.7.4凌晨测得
  receiver.throttle_ = Stick(0.050542,0.09999,0,NEGATIVE_LOGIC);
  receiver.yaw_ = Stick(0.050795,0.09999,0.75392, POSITIVE_LOGIC_BALANCED);
  receiver.pitch_ = Stick(0.049995,0.099945,0.074946,POSITIVE_LOGIC_BALANCED);
  receiver.roll_= Stick(0.049995,0.093991,0.068993,NEGATIVE_LOGIC_BALANCED);
  // PID控制器
  /* TODO
  controller.pid_pitch = PID(,,,);
  controller.pid_roll = PID(,,,);
  controller.pid_yaw = PID(,,,);
  */

  // 底层初始化
  RxBuffer[0] = '\0';
  firstinputflag = 1;
  RxIndex = 0;
  // 陀螺仪
  Init_MPU6050();


  GPIO_InitTypeDef  GPIO_InitStructure;
  // LCD
#ifdef _DEBUG_WITH_LCD
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOD , GPIO_Pin_7);    //CS=0;
  LCD_Initializtion();
  LCD_Clear(Blue);
#endif
  // 捕获器
  tim_throttle.mode_pwm_input(THROTTLE_PIN);
  tim_pitch.mode_pwm_input(PITCH_PIN);
  tim_yaw.mode_pwm_input(YAW_PIN);
  tim_roll.mode_pwm_input(ROLL_PIN);
  // 调度器
  tim_sch.mode_sch();
// 油门最高点，听到确认音
  motor1 = Motor(PWM_FREQ,MAX_DUTY,MOTOR1_PWM_TIM,MOTOR1_PWM_CH,MOTOR1_PWM_PIN);
  motor2 = Motor(PWM_FREQ,MAX_DUTY,MOTOR2_PWM_TIM,MOTOR2_PWM_CH,MOTOR2_PWM_PIN);
  motor3 = Motor(PWM_FREQ,MAX_DUTY,MOTOR3_PWM_TIM,MOTOR3_PWM_CH,MOTOR3_PWM_PIN);
  motor4 = Motor(PWM_FREQ,MAX_DUTY,MOTOR4_PWM_TIM,MOTOR4_PWM_CH,MOTOR4_PWM_PIN);
  // 电调接上电池，等待2S
#ifdef INIT_MOTOR
  Delay(DELAY_1S);
  Delay(DELAY_1S);
#endif
  // 油门推到最低，等待1S
  motor1.set_duty(MIN_DUTY);
  motor2.set_duty(MIN_DUTY);
  motor3.set_duty(MIN_DUTY);
  motor4.set_duty(MIN_DUTY);
#ifdef INIT_MOTOR
  Delay(DELAY_1S);
  Delay(DELAY_1S);
  Delay(DELAY_1S);
#endif
  // 油门最低点确认音，可以起飞

  // LED4
  RCC_AHB1PeriphClockCmd(LED4_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = LED4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(LED4_GPIO_PORT,LED4_PIN);
  motor_init_flag = 1;
}

// 调度器，每次0.5mS
void TIM8_UP_TIM13_IRQHandler(void) {
  TIM_ClearITPendingBit(tim_sch.TIM, TIM_IT_Update);
  // 接收器更新数据
  receiver.update_data(tim_throttle.DutyCycle, \
    tim_yaw.DutyCycle, \
    tim_pitch.DutyCycle, \
    tim_roll.DutyCycle);

  if (motor_init_flag) { // 只有当初始化完成才进行调度
    /*
    controller.ControlRoutine(); // 3路PID控制
    */
#ifdef _DEBUG_MOTOR
  motor1.set_duty(receiver.throttle_.convert_duty_);
  motor2.set_duty(receiver.throttle_.convert_duty_);
  motor3.set_duty(receiver.throttle_.convert_duty_);
  motor4.set_duty(receiver.throttle_.convert_duty_);
#endif
#ifdef USING_CONTROLLER

#endif
  }
}

// 接收机PWM占空比通过4个20mS中断读取
void TIM2_IRQHandler(void) {
  tim_throttle.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  uint8_t tmp3[80];
  float_to_string(tim_throttle.DutyCycle,tmp2);
  float_to_string(receiver.throttle_.convert_duty_,tmp3);
  uint8_t tmp1[80] = "throttle:";
  GUI_Text(0,0,tmp1,White,Blue);
  GUI_Text(0,LCD_LINE_SPACE,tmp2,White,Blue);
  GUI_Text(0,2*LCD_LINE_SPACE,tmp3  ,White,Blue);
#endif
}
void TIM3_IRQHandler(void) {
  tim_pitch.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  float_to_string(tim_pitch.DutyCycle,tmp2);
  uint8_t tmp1[80] = "pitch:";
  GUI_Text(0,2*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,3*LCD_LINE_SPACE,tmp2,White,Blue);
#endif
}
void TIM4_IRQHandler(void) {
  tim_yaw.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  float_to_string(tim_yaw.DutyCycle,tmp2);
  uint8_t tmp1[80] = "yaw:";
  GUI_Text(0,4*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,5*LCD_LINE_SPACE,tmp2,White,Blue);
#endif
}
void TIM5_IRQHandler(void) {
  tim_roll.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  float_to_string(tim_roll.DutyCycle,tmp2);
  uint8_t tmp1[80] = "roll:";
  GUI_Text(0,6*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,7*LCD_LINE_SPACE,tmp2,White,Blue);
#endif
}

/*
用手机上的蓝牙串口助手能够成功发送字符串
*/
void USART3_IRQHandler(void){
		char ch;
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
      ch = USART_ReceiveData(USART3);
#ifdef _DEBUG_USART
      RxBuffer[RxIndex]	= ch;
      RxIndex++;
#endif

#ifndef _DEBUG_USART // 回车有可能是0x0A，也有可能是0x0D 0x0A
      if (ch!=0x0D) {// 不接收0x0D
        /*
        算法有问题！！！这里假定了第一个字符不是回车
        千万不要只发回车，因为第一个字符一定会被接收！！！
        这个case以后再处理，现在先用着
        */
        if(firstinputflag) {
          firstinputflag = 0; // 开始组装字符串，并判断结尾0x0A
          RxIndex = 0;
          RxBuffer[RxIndex]	= ch;
          RxIndex++;
        }
        else{
          if(ch == 0x0A){
            RxBuffer[RxIndex] = '\0';
            firstinputflag = 1;	// 此次组装结束，准备下次组装
          }
          else{
            RxBuffer[RxIndex] = ch;
            RxIndex++; // 继续读下一个
          }
        }
      }
      /* TODO
      usart_cmd.Execute();
      */
#endif
    }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
