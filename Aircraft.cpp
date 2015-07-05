// 起~飞~
//#define INIT_MOTOR // 就是初始化电机，等待几秒的那些东西，试飞或者调试电机的时候需要反注释这个

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4_discovery.h"

#include "Aircraft.h"
#include "Controller.h"
#include "Formatter.h"
#include "LCD.h"
#include "Motor.h"
#include "MPU6050.h"
#include "myDelay.h"
#include "PID.h"
#include "Receiver.h"
#include "Stick.h"
#include "Timer.h"
#include "usart.h"

#include "stm32f4xx.h"
#include "util.h"
#include "usart.h"
#include "delay.h"

float pitch, roll, yaw, startup_yaw;

/* 全局变量声明 */
// 4个通道捕获计时器
Timer tim_throttle(THROTTLE_TIM); // 油门（升降） ch3
Timer tim_pitch(PITCH_TIM); // 俯仰（前后） ch2
Timer tim_yaw(YAW_TIM); // 偏航(水平转动) ch4
Timer tim_roll(ROLL_TIM); // 翻滚（左右） ch1
// 4个电机
Motor motor1;
Motor motor2;
Motor motor3;
Motor motor4;
// 调度器
Timer tim_sch(SCH_TIM);
// 通道捕获器和解释器
uint16_t Receive_Flag;
Receiver receiver;
// PID控制器
Controller controller;
// 电机初始化标志
uint8_t Aircraft_Init_Flag = 0;
// 接收缓冲区
uint8_t RxBuffer[RXBUFFLE_SIZE];
uint8_t firstinputflag;
uint8_t RxIndex;
// 运行时间计时
uint32_t system_ticks;
float system_seconds;

int main(void)
{
  MPU6050_NVIC_Configuration();
  nvic_grouping(2);
	rcc_configuration();
	delay_configuration();
	usart_configuration(UART4, 38400); // 蓝牙的波特率就是38400
	usart_configuration(USART1, 115200);
  Aircraft_Init();
	printf("RCC_Clocks.SYSCLK_Frequency = %d\r\n", rcc_clocks.SYSCLK_Frequency);
	printf("RCC_Clocks.HCLK_Frequency = %d\r\n", rcc_clocks.HCLK_Frequency);
	printf("RCC_Clocks.PCLK1_Frequency = %d\r\n", rcc_clocks.PCLK1_Frequency);
	printf("RCC_Clocks.PCLK2_Frequency = %d\r\n", rcc_clocks.PCLK2_Frequency);

  uint8_t first_time = 1;
  while (1) {
    /* 读传感器数据在while里面读 这样数据才是最新的 */
    if(flag == 1){
      if(first_time) { // first time, init the startup_yaw
        startup_yaw = yaw;
        first_time = 0;
      } // first time, init the startup_yaw
			flag = 0;
			//printf("pitch=%6.3f, roll=%6.3f, startup_yaw=%6.3f\r\n", pitch, roll, yaw-startup_yaw);
      controller.SetMeasures(pitch,roll,yaw-startup_yaw); // p, r, y
		}
    // 油门通道捕获的真实的占空比
    // throttle、yaw、roll、pitch通道捕获的真实的占空比

    // 四个通道的占空比都是能获得的，并且是正确的
    //printf("tim_throttle: %f   ",tim_throttle.DutyCycle);
    //printf("tim_yaw: %f   ",tim_yaw.DutyCycle);
    //printf("tim_roll: %f   ",tim_roll.DutyCycle);
    //printf("tim_pitch: %f   ",tim_pitch.DutyCycle);

    // 转换后的值，看看转换是不是对的
    /* throttle和roll都是对的，pitch和yaw不对，why？
    printf("receiver.throttle_.convert_duty_: %f   ",receiver.throttle_.convert_duty_);
    printf("receiver.roll_.convert_duty_: %f   ",receiver.roll_.convert_duty_);
    */
    }
  return -1;
}

uint16_t USART_Flag = 0;
#define PRINTF_TICKS 200// 0.5mS per tick
void USART_Routine(void) {
  USART_Flag ++;
  if (USART_Flag==PRINTF_TICKS) { // USART_Routine
    printf("tim_roll: %f   ",tim_roll.DutyCycle);
    receiver.update_data(tim_throttle.DutyCycle, tim_pitch.DutyCycle, tim_roll.DutyCycle, tim_yaw.DutyCycle);
    printf("receiver.roll_.convert_duty_: %f   ",receiver.roll_.convert_duty_);
    //printf("set motor1 duty: %f   ", motor1.duty());
    /* 没问题，是好的，按照这个格式打印CSV*/
    //printf("%f,%f\r\n",system_seconds,motor1.duty());
    USART_Flag = 1;
  } // USART_Routine
}

void Aircraft_Init(void) {
  /* 底层初始化 */
  RxBuffer[0] = '\0';
  firstinputflag = 1;
  RxIndex = 0;
  //Init_MPU6050(); // 陀螺仪

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
  // 油门最高点，听到确认音
  motor1 = Motor(PWM_FREQ,MAX_DUTY,MOTOR1_PWM_TIM,MOTOR1_PWM_CH,MOTOR1_PWM_PIN);
  motor2 = Motor(PWM_FREQ,MAX_DUTY,MOTOR2_PWM_TIM,MOTOR2_PWM_CH,MOTOR2_PWM_PIN);
  motor3 = Motor(PWM_FREQ,MAX_DUTY,MOTOR3_PWM_TIM,MOTOR3_PWM_CH,MOTOR3_PWM_PIN);
  motor4 = Motor(PWM_FREQ,MAX_DUTY,MOTOR4_PWM_TIM,MOTOR4_PWM_CH,MOTOR4_PWM_PIN);
  // 电调接上电池，等待2S
#ifdef INIT_MOTOR
  myDelay(DELAY_1S);
  myDelay(DELAY_1S);
#endif
  // 油门推到最低，等待1S
  motor1.set_duty(MIN_DUTY);
  motor2.set_duty(MIN_DUTY);
  motor3.set_duty(MIN_DUTY);
  motor4.set_duty(MIN_DUTY);
#ifdef INIT_MOTOR
  myDelay(DELAY_1S);
  myDelay(DELAY_1S);
  myDelay(DELAY_1S); // 油门最低点确认音，可以起飞
#endif
  // 调度器
  tim_sch.mode_sch();
  // LED4
  RCC_AHB1PeriphClockCmd(LED4_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = LED4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(LED4_GPIO_PORT,LED4_PIN);
  // 运行时间计时
  system_ticks = 0;
  system_seconds = 0.0;

  /* 控制层初始化 */
  Receiver_Init(); // 接收机
  Controller_Init(); // 3通道PID控制器

  /* 全部初始化完成 */
  Aircraft_Init_Flag = 1;
}

// 调度器 SCH ，每次0.5mS
void TIM8_UP_TIM13_IRQHandler(void) {
  TIM_ClearITPendingBit(tim_sch.TIM, TIM_IT_Update);
  if (Aircraft_Init_Flag) { // if Aircraft_Init_Flag 只有当初始化全部完成才进行调度
    system_ticks ++;
    system_seconds = SCHEDULER_TICK * system_ticks;
    Receive_Flag ++; // Receiver Routine Flag
    if (Receive_Flag == RECEIVE_TICK) { // 15mS调度一次
      receiver.update_data(tim_throttle.DutyCycle, tim_pitch.DutyCycle, tim_roll.DutyCycle, tim_yaw.DutyCycle);
      //printf("receiver.yaw_.convert_duty_: %f   ",receiver.yaw_.convert_duty_); // 测试定时器时间准不准用的
      // 获取油门大小
      controller.throttle(receiver.throttle_.convert_duty_); // 输入油门，0.05~0.10
      /*
        PITCH，-1~+1，向前为正，向后为负
        ROLL，-1~+1，向右下为正，向左下为负（很好理解，因为是右手系）
        YAW，-1~+1，逆时针/向左为正，顺时针/向右为负
      */
      controller.SetPoints(receiver.pitch_.convert_duty_,\
        receiver.roll_.convert_duty_,\
        receiver.yaw_.convert_duty_); // pitch, roll, yaw
      Receive_Flag = 1;
    } // 15mS调度一次

    // 陀螺仪接口在主函数里面
    // controller.SetMeasures(pitch,roll,yaw-startup_yaw); // p, r, y
    controller.Routine(); // 3路PID控制计算，里面会自动分频
    if (controller.IsExecuted()) {
      float m1d = controller.motor1_duty();
      float m2d = controller.motor2_duty();
      float m3d = controller.motor3_duty();
      float m4d = controller.motor4_duty();
      motor1.set_duty(m1d);
      motor2.set_duty(m2d);
      motor3.set_duty(m3d);
      motor4.set_duty(m4d);
    }
    /* 打印例程 */
    USART_Routine();
#ifdef _DEBUG_MOTOR
    // 输入给电机的是转换之后的占空比
    motor1.set_duty(receiver.throttle_.convert_duty_);
    motor2.set_duty(receiver.throttle_.convert_duty_);
    motor3.set_duty(receiver.throttle_.convert_duty_);
    motor4.set_duty(receiver.throttle_.convert_duty_);
#endif
  } // if Aircraft_Init_Flag
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

void Receiver_Init(void) {
  Receive_Flag = 1;
  // 接收机
  // 数据在2015.7.4晚上更新
  // 油门，负逻辑，无平衡点
  receiver.throttle_ = Stick(0.050495,0.099845,0,NEGATIVE_LOGIC);
  // 偏航，正逻辑，有平衡点，需要归一化（-1~1）
  receiver.yaw_ = Stick(0.050395,0.09999,0.075446, POSITIVE_LOGIC_BALANCED_UNITARY);
  // 俯仰，负逻辑，有平衡点，需要归一化
  receiver.pitch_ = Stick(0.049995,0.099945,0.074993,NEGATIVE_LOGIC_BALANCED_UNITARY);
  // 翻滚，负逻辑，有平衡点，需要归一化
  receiver.roll_= Stick(0.049995,0.093991,0.068993,NEGATIVE_LOGIC_BALANCED_UNITARY);
}

void Controller_Init(void) {
  controller = Controller(CONTROL_FREQ, SCHEDULER_TICK);
  float dt = 1.0 / CONTROL_FREQ;
  controller.pid_pitch = PID(PID_PITCH_KP,PID_PITCH_KI,PID_PITCH_KD,dt,-MAX_I_ABS,MAX_I_ABS);
  controller.pid_roll = PID(PID_ROLL_KP,PID_ROLL_KI,PID_ROLL_KD,dt,-MAX_I_ABS,MAX_I_ABS);
  controller.pid_yaw = PID(PID_YAW_KP,PID_YAW_KI,PID_YAW_KD,dt,-MAX_I_ABS,MAX_I_ABS);
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
