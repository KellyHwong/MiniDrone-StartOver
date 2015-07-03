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

class PID() {
 private:
  // 调度设置
  unsigned int interval_unit_; // 调度时间单位，in mS，用来各个输出计算
  // 与调度器有关
  unsigned int interval_ticks_; // 调度间隔，in ticks
  unsigned int interval_counter_; // 调度计数
  // 控制设置
  float Kp_;
  float Ki_;
  float Kd_;
  // 反馈设置
  float setpoint_;
  float pre_error_; //上一个误差
  float error_; // 误差
  float d_; // 误差的差分
  float i_; // 误差的积分
  // 输出
  float pout_;
  float iout_;
    // 积分限幅
    float imax_;
    float imin_;
  float dout_;
  float out_;
  void EvalOutput(void); // 计算三个输出
  // set 函数
  /* float pout(float p_o);
  float iout(float i_o);
  float dout(float d_o); */
 public:
  PID();
  // set函数
  void error(float e); // 输入误差接口
  // get输出
  float pout(); // p端输出
  float iout(); // i端输出
  float dout(); // d端输出
  float out(); // 总输出
  float iminmax(float min, max); // 设置积分限幅
}

#ifdef __cplusplus
}
#endif

#endif
