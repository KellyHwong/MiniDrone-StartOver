#include "Receiver.h"

Receiver::Receiver() {}

/*void Receiver::update_data(float throttle) {
  throttle_.get_now_duty(throttle);
}*/

// 被调度器调度的
void Receiver::update_data(float t, float p, float r, float y) {
  throttle_.data_convert(t);
  
  yaw_.data_convert(y);
  
  pitch_.data_convert(p);
  
  roll_.data_convert(r);
}
