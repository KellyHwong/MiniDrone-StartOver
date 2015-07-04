#include "Utilities/Receiver.h"

Receiver::Receiver() {}

void Receiver::update_data(float throttle) {
  throttle_.get_now_duty(throttle);
}

void Receiver::update_data(float throttle, float yaw, float pitch, float roll) {
  throttle_.get_now_duty(throttle);
  yaw_.get_now_duty(yaw);
  pitch_.get_now_duty(pitch);
  roll_.get_now_duty(roll);
}
