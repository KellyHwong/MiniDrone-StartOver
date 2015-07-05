/**
* @Filename: PID.cpp
  error_ = 0;

* @Update: 2015.6.27
* @Description: 跨平台通用类，PID反馈控制器
*/
#include "PID.h"

#define DEFAULT_MAX_I_ABS 10000// 默认积分幅值（绝对值），设置为1W，足够大了吧

// 默认构造函数
PID::PID() {
  setpid(0,0,0);
  dt_ = 0.01; // 不能初始化为0！否则会除以0
  setpoint_ = 0;
  pre_error_ = error_ = 0;
  error_i_ = 0;
  set_i_limit(-DEFAULT_MAX_I_ABS, DEFAULT_MAX_I_ABS);
  EvalError(0); // measured 0
  EvalOut();
}

// 输入参数：PID三个参数，时间间隔
PID::PID(float Kp, float Ki, float Kd, float dt) {
  setpid(Kp,Ki,Kd);
  dt_ = dt;
  setpoint(0);
  pre_error_ = error_ = 0;
  error_i_ = 0;
  set_i_limit(-DEFAULT_MAX_I_ABS, DEFAULT_MAX_I_ABS);
  EvalError(0);
  EvalOut();
}

// 输入参数：PID三个参数，时间间隔，以及积分限幅
PID::PID(float Kp, float Ki, float Kd, float dt, float imin, float imax) {
  setpid(Kp,Ki,Kd);
  dt_ = dt; // 一定记得输入非0的数！否则除0异常！
  setpoint(0);
  pre_error_ = error_ = 0;
  error_i_ = 0;
  set_i_limit(imin, imax);
  EvalError(0);
  EvalOut();
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
  error_i_ += error_ * dt_;
  // 积分要限幅
  limit_i();
  error_d_ = (error_ - pre_error_) / dt_;
  pre_error_ = error_;
}

// 计算P I D各路输出，以及总输出
void PID::EvalOut(void) {
  pout_ = Kp_ * error_;
  iout_ = Ki_ * error_i_;
  dout_ = Kd_ * error_d_;
  out_ = pout_ + iout_ + dout_;
}

// PID的一次例程，要输入测量的数据，感觉这么写不好
void PID::Routine(float measured_) {
  EvalError(measured_); // 计算误差
  EvalOut(); // 计算输出
}

// setter functions
// 设置setpoint
void PID::setpoint(float sp) {
  setpoint_ = sp;
}
// 设置积分限幅
inline void PID::set_i_limit(float imin, float imax) {
  imin_ = imin;
  imax_ = imax;
}
// 积分要限幅
inline void PID::limit_i() {
  if (error_i_ > imax_) {
    error_i_ = imax_;
  }
  else if (error_i_ < imin_) {
    error_i_ = imin_;
  }
}
// getter functions
float PID::setpoint(void) {
  return setpoint_;
}
float PID::pout(void) {
  return pout_;
}
float PID::iout(void) {
  return iout_;
}

float PID::dout(void) {
  return dout_;
}
float PID::out(void) {
  return out_;
}
