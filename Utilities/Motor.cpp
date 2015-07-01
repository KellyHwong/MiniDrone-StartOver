#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"
#include "Utilities/Motor.h"

Motor::Motor() {}

Motor::Motor(PWM pwm) {
  pwm_ = PWM(pwm);
}

// 输入PWM占空比限幅
float Motor::duty_limit(float duty) {
  if (duty < MIN_DUTY)
    duty = MIN_DUTY;
  if (duty > MAX_DUTY)
    duty = MAX_DUTY;
  return duty;
}

void Motor::set_duty(float duty) {
  duty_ = duty_limit(duty);
  pwm_.set_duty(duty_);
}

float Motor::get_duty(void) {
  return duty_;
}
