#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Timer.h"
#include "Pin.h"
#include "Formatter.h"
#include "LCD.h"
#include "Motor.h"

// 是对的，是对的
// TODO 电机启动后不能小于这是值
// 不过庄总说可以先不做，毕竟有PID，而且逻辑有点复杂
#define STARTUP_DUTY 0.0526 // 测试出来的最小电机启动占空比

Motor::Motor() {}

Motor::Motor(uint32_t freq, float duty, uint8_t TIM_No, uint8_t CH_No,PinTypedef pin) {
  pwm_ = PWM(freq, duty, TIM_No, CH_No, pin);
}

// 输入PWM占空比限幅
float Motor::limit_duty(float duty) {
  if (duty < MIN_DUTY)
    duty = MIN_DUTY;
  if (duty > MAX_DUTY)
    duty = MAX_DUTY;
  return duty;
}

void Motor::set_duty(float duty) {
  duty_ = limit_duty(duty);
  pwm_.set_duty(duty_);
}

float Motor::duty(void) {
  return duty_;
}
