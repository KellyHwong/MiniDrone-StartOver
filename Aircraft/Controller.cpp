#include <stdio.h>
#include "Aircraft.h"
#include "Controller.h"
#include "PID.h"

#define MAX_ANGLE 8.333
#define MAX_DUTY_BEFORE_MUILTIPLIED_BY_K 0.025 // 这他妈再能理解了吧！！！
#define DEFAULT_ROUTINE_FREQ 100
#define DEFALT_SCHEDULER_TICK 0.001 // 默认的调度器时间间隔，没什么用，只是防止错误初始化

Controller::Controller(void) {
  routine_freq_ = DEFAULT_ROUTINE_FREQ;
  scheduler_tick_ = DEFALT_SCHEDULER_TICK;
  routine_flag_ = 1.0 / routine_freq_ / scheduler_tick_;
  routine_flag_int_ = (uint8_t)routine_flag_;
  routine_counter_ = 1;

  motor1_duty_ = 0;
  motor2_duty_ = 0;
  motor3_duty_ = 0;
  motor4_duty_ = 0;
}

// 输入的调度器基准时间单位是S！！！
Controller::Controller(float routine_freq, float scheduler_tick) {
  routine_freq_ = routine_freq;
  scheduler_tick_ = scheduler_tick;
  routine_flag_ = 1.0 / routine_freq_ / scheduler_tick_;
  routine_flag_int_ = (uint8_t)routine_flag_;
  routine_counter_ = 1;

  motor1_duty_ = 0;
  motor2_duty_ = 0;
  motor3_duty_ = 0;
  motor4_duty_ = 0;
}

inline void Controller::SetPoints(float p, float r, float y) {
  // 把角度控制值（setpoint）传给PID，PID负责计算误差和PID输出
  pid_pitch.setpoint(p);
  pid_roll.setpoint(r);
  pid_yaw.setpoint(y);
}

// 输入现在的三个量，数据从传感器获得
inline void Controller::SetMeasures(float measured_pitch, float measured_row, float measured_yaw) {
  measured_pitch_ = measured_pitch;
  measured_row_ = measured_row;
  measured_yaw_ = measured_yaw;
}

void Controller::Routine(void) { // 控制器控制例程（用调度器调度）
  routine_counter_ ++;
  /* 调度控制算法，仅当计数达到相应值时调度 */
  if (routine_counter_==routine_flag_int_) {
    // 三个PID计算例程
    pid_pitch.Routine(measured_pitch_);
    pid_roll.Routine(measured_row_);
    pid_yaw.Routine(measured_yaw_);
    // 反馈量的获取
    float pitch_out = pid_pitch.out();
    float row_out = pid_roll.out();
    float yaw_out = pid_yaw.out();
    /*
    四个电机的设置，参考Aircraft.h和电路板
    2     1
      \ /
       X
      / \
    3     4
    */
    duty = stick_control * MAX_DUTY_BEFORE_MUILTIPLIED_BY_K; // 传参数进来，stick_control，-1 ~ +1
// stick 归一化之后 input 进来就是-1 ~ +1，乘上最大角度8.333，就是我希望的角度，并且作为角度控制PID的输入
// （前面是把角度输入给PID，这个是做duty的换算）再除以最大角度8.333，乘以最大（也不叫最大，因为后面要乘以KP再
// 输入给Motor，）
    /* TODO 待会再写
    // 以油门为基准
    motor1_duty_ = throttle_ +
    motor2_duty_ = throttle_ +
    motor3_duty_ = throttle_ +
    motor4_duty_ = throttle_ +
    */
    /* 并把routine_counter置回1 */
    routine_counter_ = 1;
  }
}

// getter functions 获得计算出来的电机的占空比
inline float Controller::motor1_duty(void) {
  return motor1_duty_;
}
inline float Controller::motor2_duty(void) {
  return motor2_duty_;
}
inline float Controller::motor3_duty(void) {
  return motor3_duty_;
}
inline float Controller::motor4_duty(void) {
  return motor4_duty_;
}
// setter functions 输入油门
inline void Controller::throttle(float th) {
  throttle_ = th;
}
inline uint8_t Controller::IsExecuted(void) {// 返回给调度器
  return routine_counter_==routine_flag_int_;
}
