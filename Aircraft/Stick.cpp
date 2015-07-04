#include "Utilities/Stick.h"

#define MIN_STICK_DUTY 0.05
#define MAX_STICK_DUTY 0.10
#define MID_STICK_DUTY 0.075
#define DUTY_INTERVAL 0.05

Stick::Stick() {
  min_duty_ = MIN_STICK_DUTY;
  max_duty_ = MAX_STICK_DUTY;
  balance_duty_ = MID_STICK_DUTY;
  mode_ = NEGATIVE_LOGIC;
}

Stick::Stick(float min_duty, float max_duty, float balance_duty, DATA_MODE mode) {
  min_duty_ = min_duty;
  max_duty_ = max_duty;
  balance_duty_ = balance_duty;
  mode_ = mode;
}

// 调用这个即可
void Stick::seft_convert(void) {
  seft_standardize();
  seft_invert();
}

void Stick::seft_invert(void) {
  switch (mode_) {
    // 负逻辑的就要方向
    case (NEGATIVE_LOGIC):
    case (NEGATIVE_LOGIC_BALANCED): {
      convert_duty_ = MAX_STICK_DUTY + MIN_STICK_DUTY - std_duty_;
      break;
    }
  }
}

void Stick::seft_standardize(void) {
  switch (mode_) {
    // 没有平衡点
    case (NEGATIVE_LOGIC): {
      // 归一化到5.0~10.0
      std_duty_ = (now_duty_-min_duty_)/(max_duty_-min_duty_)*DUTY_INTERVAL + MIN_STICK_DUTY;
      break;
    }
    // 有平衡点
    case (NEGATIVE_LOGIC_BALANCED):
    case (POSITIVE_LOGIC_BALANCED):{
      // 大于平衡点的归一化到0.075~0.10
      if (now_duty_ > balance_duty_){
        std_duty_ = (now_duty_-balance_duty_)/(max_duty_-balance_duty_)*DUTY_INTERVAL/2 + MID_STICK_DUTY;
      }
      // 小于平衡点的归一化到0.05~0.075
      else {
        std_duty_ = (now_duty_-min_duty_)/(balance_duty_-min_duty_)*DUTY_INTERVAL/2 + MIN_STICK_DUTY;
      }
      break;
    }
  }
}

// 数据接收接口
void Stick::get_now_duty(float now_duty) {
  now_duty_ = now_duty;
  this->seft_convert();
}
