/**
* @Filename: PID.cpp
* @Author: Kelly Hwong
* @Update: 2015.6.27
* @Description: 跨平台通用类，PID反馈控制器
*/
#include "PID.h"

#define DEFAULT_MAX_I_ABS 10000// 默认积分幅值（绝对值），设置为1W，足够大了吧

// 默认构造函数
PID::PID() {

}

// 输入参数：PID三个参数，时间间隔
PID::PID(float Kp, float Ki, float Kd, float dt) {
  Kp_ = Kp;
  Ki_ = Ki;
  Kd_ = kd;
  dt_ = dt;
}

// 设置PID参数
void PID::setpid(float Kp, float Ki, float Kd) {
  Kp_ = Kp;
  Ki_ = Ki;
  Kd_ = Kd;
}

// 计算误差，注意要先初始化setpoint！！！
void PID::EvalError(float measured_value) {
  error_ = setpoint_ - measured_value;
  error_i_ += error_ *
}
// 重载
void PID::EvalError(float setpoint, float measured_value) {
  setpoint_ = setpoint;

}

// 计算P I D各路输出，以及总输出
void EvalOut(void) {
  pout_ = Kp_ * ;
  iout_ = 1;
  dout_ = 1;
}

// PID控制例程
void PID::control(void) {
  e_ = setpoint_ - (*ptr_measured);
  i_ += e_ * interval_unit_ / 1000;
  d_ = (e_-pre_e_) / interval_unit_ * 1000;
  pre_e_ = e_;
  (*ptr_con_var_) = setpoint_ + Kp_*e_ + Ki_*i_ + Kd*d_;
}

// setter functions
// 设置setpoint
inline void PID::setpoint(float setpoint) {
  setpoint_ = setpoint;
}

// getter functions
inline float PID::pout(void) {
  return pout_;
}
inline float PID::iout(void) {
  return iout_;
}

inline float PID::dout(void) {
  return dout_;
}
inline float PID::out(void) {
  return out_;
}
