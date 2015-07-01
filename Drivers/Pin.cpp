#include <stdint.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "Pin.h"

// 不复用
Pin::Pin(PinTypedef pin) {
  this.eval_properties(pin); // 计算各个成员的值
  this.GPIO_AF_TIM_ = 0xFF; // 不复用
}

// 复用，用这个初始化
Pin::Pin(PinTypedef pin, uint8_t GPIO_AF_TIM) {
  this.eval_properties(pin);
  this.GPIO_AF_TIM_ = GPIO_AF_TIM;
}

Pin::Pin_Init(PinTypedef pin) {
  // 创造了它就要对它负责，让它可以启动

}

void Pin::eval_properties(PinTypedef pin) {
  this.GPIO_PinSource_ = pin % 16;
  this.GPIO_Pin_ = (1 << this.GPIO_PinSource_);
  switch (this.GPIO_Pin_/16) {
    case (0): { this.GPIO_ = GPIOA; break; }
    case (1): { this.GPIO_ = GPIOB; break; }
    case (2): { this.GPIO_ = GPIOC; break; }
    case (3): { this.GPIO_ = GPIOD; break; }
    case (4): { this.GPIO_ = GPIOE; break; }
    case (5): { this.GPIO_ = GPIOF; break; }
    case (6): { this.GPIO_ = GPIOG; break; }
    case (7): { this.GPIO_ = GPIOH; break; }
    case (8): { this.GPIO_ = GPIOI; break; }
  }
}

uint16_t Pin::GPIO_Pin() {
  return this.GPIO_Pin_;
}
GPIO_TypeDef * Pin::GPIO() {
  return this.GPIO_;
}
uint8_t Pin::GPIO_PinSource() {
  return this.GPIO_PinSource_;
}
uint8_t Pin::GPIO_AF_TIM() {
  return this.GPIO_AF_TIM_;
}
