#ifndef RECEIVER_H_
#define RECEIVER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "Stick.h"

class Receiver {
 private:
 public:
  Stick throttle_; // 油门
  Stick yaw_; // 偏航
  Stick pitch_; // 俯仰
  Stick roll_; // 翻滚
  Receiver();
  void update_data(float throttle);
  void update_data(float throttle, float yaw, float pitch, float roll);
}; // class Stick

#ifdef __cplusplus
}
#endif

#endif
