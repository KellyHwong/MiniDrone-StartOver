#include "Controller.h"
#include "Aircraft.h"

#define MAX_ANGLE 8.333
#define MAX_DUTY_BEFORE_MUILTIPLIED_BY_K 0.025 // 这他妈再能理解了吧！！！
#define ROUTINE_FREQ 200 // 调度频率，单位Hz




duty = stick_control * MAX_DUTY_BEFORE_MUILTIPLIED_BY_K; // 传参数进来，stick_control，-1 ~ +1
// stick 归一化之后 input 进来就是-1 ~ +1，乘上最大角度8.333，就是我希望的角度，并且作为角度控制PID的输入
// （前面是把角度输入给PID，这个是做duty的换算）再除以最大角度8.333，乘以最大（也不叫最大，因为后面要乘以KP再
// 输入给Motor，）
PID;

Controller::Controller(void) {
  routine_flag_ = 1.0 / ROUTINE_FREQ / SCHEDULER_TICK;
  routine_flag_int_ = (uin8_t)routine_flag_;
  routine_counter_ = 1;
}

// 输入现在的三个量，数据从传感器获得
void Controller::GetMeasures(float measured_pitch, float measured_row, float measured_yaw) {
  measured_pitch_ = measured_pitch;
  measured_row_ = measured_row;
  measured_yaw_ = measured_yaw;
}

void Controller::Routine(void) { // 控制器控制例程（用调度器调度）
  routine_counter_ ++;
  if (routine_counter_==routine_flag_int_) {
    /* 调度控制算法 */
    // PID计算误差
    pid_pitch.EvalError(measured_pitch);
    pid_row.EvalError(measured_row);
    pid_yaw.EvalError(measured_yaw);
    // PID计算输出
    pid_pitch.EvalOut();
    pid_row.EvalOut();
    pid_yaw.EvalOut();
    // 四个电机的控制
    /* TODO 待会再写
    motor1_duty_ =
    motor2_duty_ =
    motor3_duty_ =
    motor4_duty_ =
    */
    /* 并把routine_counter置回1 */
    routine_counter_ = 1;
  }
}

// getter functions 获得计算出来的电机的占空比
inline float motor1_duty(void) {
  return motor1_duty_;
}
inline float motor2_duty(void) {
  return motor2_duty_;
}
inline float motor3_duty(void) {
  return motor3_duty_;
}
inline float motor44_duty(void) {
  return motor4_duty_;
}

