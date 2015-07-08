// 起~飞~
//#define INIT_MOTOR // 就是初始化电机，等待几秒的那些东西，试飞或者调试电机的时候需要反注释这个

#define PID_PITCH
#define PID_ROLL
#define PID_YAW

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4_discovery.h"

#include "Aircraft.h"
#include "BluetoothCmd.h"
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

float pitch, roll, yaw, startup_pitch, startup_roll, startup_yaw, setm_pitch, setm_roll, setm_yaw;
float startup_pitch_convert_duty, startup_roll_convert_duty, startup_yaw_convert_duty;

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
uint8_t RxBufferReady;
// 运行时间计时
uint32_t running_ticks;
float running_seconds;
// 字符串命令解释/执行器
BluetoothCmd bluetoothcmd(); // 传函数地址

int main(void)
{
  rcc_configuration();
  nvic_configuration(USART1_IRQn,2,0);
  nvic_configuration(UART4_IRQn,2,0);
	//delay_configuration();
	usart_configuration(UART4, 38400); // 蓝牙的波特率就是 38400
	usart_configuration(USART1, 115200); // 这个用来响应MPU6050的发送数据请求中断
  Aircraft_Init();

  uint8_t first_time = 1;
  while (1) {
    /* 读传感器数据在while里面读 这样数据才是最新的 */
    if(flag == 1){
      if(first_time) { // first time, init the startup_yaw
        startup_pitch = pitch;
        startup_roll = roll;
        startup_yaw = yaw;
        first_time = 0;
      } // first time, init the startup_yaw
			flag = 0;
      /* DEBUG 角度校准 */
      setm_pitch = pitch - startup_pitch;
      setm_roll = roll - startup_roll;
      setm_yaw = yaw - startup_yaw; /* 等等, 减出来不一定0~360 */
      controller.SetMeasures(setm_pitch, setm_roll, setm_yaw); // p, r, y
		}
  }
  return -1;
}

uint16_t USART_Flag = 0;
uint8_t Fisrt_Print = 1;

#define PRINTF_TICKS 200// 0.5mS per tick

void USART_Routine(void) {
  USART_Flag ++;
  if (USART_Flag==PRINTF_TICKS) { // USART_Routine
    /* 在这里写下需要打印的东西 */
    if (RxBufferReady==1) {
      printf("%s\r\n", RxBuffer); /* USART Received: 我要看到是16进制, 这句话不要 */
      RxBufferReady = 0;
    }
    // USART_Routine
    USART_Flag = 1;
  } // USART_Routine
}

void Aircraft_Init(void) {
  /* 底层初始化 */
  // 接收缓存
  RxBuffer[0] = '\0';
  firstinputflag = 1;
  RxIndex = 0;
  RxBufferReady = 0;
  // PWM 捕获器
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
  // 运行时间计时
  running_ticks = 0;
  running_seconds = 0.0;

  /* 控制层初始化 */
  Receiver_Init(); // 接收机
  Controller_Init(); // 3通道PID控制器
#ifdef INIT_MOTOR
  myDelay(DELAY_1S*4); // 等待足够长的时间，确认初始各个角度值（用于校准）
#endif
  startup_pitch_convert_duty = receiver.pitch_.convert_duty_;
  startup_roll_convert_duty = receiver.roll_.convert_duty_;
  startup_yaw_convert_duty = receiver.yaw_.convert_duty_;

  bluetoothcmd = BluetoothCmd(&adjust_p);
  /* 全部初始化完成 */
  Aircraft_Init_Flag = 1;
}

// 调度器 SCH ，每次0.5mS
void TIM8_UP_TIM13_IRQHandler(void) {
  TIM_ClearITPendingBit(tim_sch.TIM, TIM_IT_Update);
  if (Aircraft_Init_Flag) { // if Aircraft_Init_Flag 只有当初始化全部完成才进行调度
    running_ticks ++;
    running_seconds = SCHEDULER_TICK * running_ticks;
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
      controller.SetPoints(receiver.pitch_.convert_duty_ - startup_pitch_convert_duty,\
        receiver.roll_.convert_duty_ - startup_roll_convert_duty,\
        receiver.yaw_.convert_duty_ - startup_yaw_convert_duty); // pitch, roll, yaw
      Receive_Flag = 1;
    } // 15mS调度一次

    // 陀螺仪接口在主函数里面
    // controller.SetMeasures(pitch,roll,yaw); // p, r, y
    controller.Routine(); // 3路PID控制计算，里面会自动分频
    if (controller.IsExecuted()) {
      // 这样逻辑清楚一点
      if (controller.throttle()<MOTOR_STARTUP_DUTY) {
        motor1.on(0);
        motor2.on(0);
        motor3.on(0);
        motor4.on(0);
      }
      else {
        motor1.on(1);
        motor2.on(1);
        motor3.on(1);
        motor4.on(1);
      }
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
  } // if Aircraft_Init_Flag
}

// 接收机PWM占空比通过4个20mS中断读取
void TIM2_IRQHandler(void) {
  tim_throttle.PWM_Input_Handler();
}
void TIM3_IRQHandler(void) {
  tim_pitch.PWM_Input_Handler();
}
void TIM4_IRQHandler(void) {
  tim_yaw.PWM_Input_Handler();
}
void TIM5_IRQHandler(void) {
  tim_roll.PWM_Input_Handler();
}

/*
用手机上的蓝牙串口助手能够成功发送字符串
*/
void UART4_IRQHandler(void) {
		uint8_t ch;
		if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
      ch = USART_ReceiveData(UART4);
#ifdef _DEBUG_USART
      RxBuffer[RxIndex]	= ch;
      RxIndex++;
#endif

#ifndef _DEBUG_USART // 回车有可能是0x0A，也有可能是0x0D 0x0A
      if (1) { // 使用蓝牙串口协议, 要接收所有字符, 包括checksum. last ver: //(ch!=0x0D) {// 不接收0x0D
        /*
          算法有问题！！！这里假定了第一个字符不是回车. 千万不要只发回车，因为第一个字符一定会被接收！！！这个case以后再处理，现在先用着
        */
        if(firstinputflag&&((RxBuffer[0]=ch)==0xA5)) {
          firstinputflag = 0; // //开始组装字符串，并判断结尾0x0A
          RxBufferReady = 0;
          /* 清空接收区域 */
          for (int i=0; i<RXBUFFLE_SIZE; i++) { RxBuffer[i] = '\0'; }
          RxIndex = 0;
          RxBuffer[RxIndex]	= ch;
          RxIndex++;
        }
        else{
          /* 0x0A 是我的帧结尾, 即回车, 0xAA 是蓝牙串口协议的帧结尾 */
          if ( (ch == 0xAA)&&(RxIndex==6) ) { // 6+1 帧长度 last ver: (ch == 0x0A)
            RxBuffer[RxIndex] = '\0';
            firstinputflag = 1;	// 此次组装结束，准备下次组装
            RxBufferReady = 1; // 准备好了
          }
          else{
            RxBuffer[RxIndex] = ch;
            RxIndex++; // 继续读下一个
            /* 溢出判断 */
            if(RxIndex > 6){ // 传输错误
              RxBufferReady = 0;
              for (int i=0; i<RXBUFFLE_SIZE; i++) { RxBuffer[i] = '\0'; }
              firstinputflag = 1;
              RxIndex = 0;
            }
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
  float dt = 1.0 / CONTROL_FREQ; // 频率分之1
  controller.pid_pitch = PID(PID_PITCH_KP,PID_PITCH_KI,PID_PITCH_KD,dt,-MAX_I_ABS,MAX_I_ABS);
  controller.pid_roll = PID(PID_ROLL_KP,PID_ROLL_KI,PID_ROLL_KD,dt,-MAX_I_ABS,MAX_I_ABS);
  controller.pid_yaw = PID(PID_YAW_KP,PID_YAW_KI,PID_YAW_KD,dt,-MAX_I_ABS,MAX_I_ABS);
}

/* 程序结构还是差了点, 因为把controller作为全局变量放在Aircraft里面
*  调节pitch的PID参数 (现在是调P参数)
* @params: value: the value in the string protocol
*
*/
#define WITCH_PID_TO_ADJUST pid_pitch // 你要干哪个 PID?
void adjust_p(uint8_t value) {
  float min = 0;  // 参数最小值
  float max = 10; // 参数最大值
  float value_f = (float)value; // 0~255 转浮点数
  value_f = value_f/(255-0)*(max-min); // scaling
  controller.WITCH_PID_TO_ADJUST.Kp(value_f); // 调用PID的参数设置接口
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
