/**
* @Filename: PID.cpp
* @Author: Kelly Hwong
* @Update: 2015.6.27
* @Description: 跨平台通用类，PID反馈控制器
*/
#ifndef PID_H_
#define PID_H_

#ifdef __cplusplus
 extern "C" {
#endif

class PID {
 private:
  // 调度设置
  /* PID类不负责调度设置，调度设置外外部Control类中实现，PID只要负责自己的事情
  PID自己的事情就几个，输入期望（比如角度值），输入测量值，Eval误差（error_,pre_error_,error_d_,error_i_）
  输出反馈值（还是角度）
  unsigned int interval_unit_; // 调度时间单位，in mS，用来各个输出计算
  // 与调度器有关
  unsigned int interval_ticks_; // 调度间隔，in ticks
  unsigned int interval_counter_; // 调度计数
  */
  // 控制设置
  float Kp_;
  float Ki_;
  float Kd_;
  float dt_; // 时间间隔
  // 反馈设置
  float setpoint_;
  // 误差计算
  float pre_error_; //上一个误差
  float error_; // 误差
  float error_d_; // 误差的差分
  float error_i_; // 误差的积分
  // 输出
  float pout_; // P路输出
  float iout_; // I路输出
    // 积分限幅
    float imax_;
    float imin_;
  float dout_; // D路输出
  float out_; // 总输出

  void EvalError(float measured_value);
  void EvalOut(void);
  inline void limit_i();
 public:
  PID();
  PID(float Kp, float Ki, float Kd, float dt);
  PID(float Kp, float Ki, float Kd, float dt, float imin, float imax);
  inline void Routine(float measured_);
  void setpid(float Kp, float Ki, float Kd);
  inline void set_i_limit(float imin, float imax);
  inline void setpoint(float sp);
  // get输出
  inline float pout(void);
  inline float iout(void);
  inline float dout(void);
  inline float out(void);
}; // class PID

#ifdef __cplusplus
}
#endif

#endif
