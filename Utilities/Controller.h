/**
* @Filename: Controller.h
* @Author: Kelly Hwong
* @Update: 2015.7.2
* @Description: 飞行器姿态控制器
*/

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "Utilities/Controller.h"

class Controller {
 private:
  // PID pid_throttle_;
  PID pid_yaw_;
  PID pid_pitch_;
  PID pid_roll_;
 public:
  Controller();
  void EvalParams(float throttle, float yaw, float pitch, float roll); // 计算角度等参数
  void ControlRoutine(void); // 控制器控制例程（用调度器调度）
}; // class Stick

#ifdef __cplusplus
}
#endif

#endif
