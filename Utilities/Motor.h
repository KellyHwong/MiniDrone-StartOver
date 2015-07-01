#ifndef MOTOR_H_
#define MOTOR_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Drivers/PWM.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"
   
#define MAX_DUTY 0.10
#define MIN_DUTY 0.05

class Motor {
 private:
  PWM pwm_;
  float duty_;
  float duty_limit(float duty);
 public:
  Motor();
  Motor(PWM pwm);
  void set_duty(float duty);
  float get_duty(void);
}; // class Motor

#ifdef __cplusplus
}
#endif

#endif
