#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Timer.h"
#include "Pin.h"
#include "Formatter.h"
#include "LCD.h"
#include "Motor.h"

#define MOTOR_MAX_RUNNING_DUTY 0.10
#define MOTOR_MIN_DUTY 0.05

// 是对的，是对的
// TODO 电机启动后不能小于这是值
// 不过庄总说可以先不做，毕竟有PID，而且逻辑有点复杂

Motor::Motor() {}

Motor::Motor(uint32_t freq, float duty, uint8_t TIM_No, uint8_t CH_No,PinTypedef pin) {
  pwm_ = PWM(freq, duty, TIM_No, CH_No, pin);
  on_ = 0; // 关闭电机
}

// 输入PWM占空比限幅
float Motor::limit_duty(float duty) {
  if (this->on_) { // 打开的话
    if (duty < MOTOR_STARTUP_DUTY) // 则不低于电机停转的最小占空比
      duty = MOTOR_STARTUP_DUTY;
    if (duty > MOTOR_MAX_RUNNING_DUTY)
      duty = MOTOR_MAX_RUNNING_DUTY;
  }
  else {
    duty = MOTOR_MIN_DUTY; // 没打开就一直保持最低占空比
  }
  return duty;
}

void Motor::set_duty(float duty) {
  duty_ = limit_duty(duty);
  pwm_.set_duty(duty_);
}

float Motor::duty(void) {
  return duty_;
}

uint8_t Motor::on(void) {
  return on_;
}

void Motor::on(uint8_t o) {
  this->on_ = o;
}
