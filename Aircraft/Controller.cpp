#include "Utilities/Controller.h"

#define MAX_ANGLE 8.333

Controller::Controller() {}

void Controller::update_data(float throttl#include "Utilities/Controller.h"

Controller::Controller() {}

void Controller::update_data(float throttle) {
  throttle_.get_now_duty(throttle);
}

void Controller::update_data(float throttle, float yaw, float pitch, float roll) {
  throttle_.get_now_duty(throttle);
  yaw_.get_now_duty(yaw);
  pitch_.get_now_duty(pitch);
  roll_.get_now_duty(roll);
}
e) {
  throttle_.get_now_duty(throttle);
}

void Controller::update_data(float throttle, float yaw, float pitch, float roll) {
  throttle_.get_now_duty(throttle);
  yaw_.get_now_duty(yaw);
  pitch_.get_now_duty(pitch);
  roll_.get_now_duty(roll);
}
