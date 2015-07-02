#include "Utilities/Stick.h"

#define STD_MIN 5.0
#define STD_MAX 10.0
#define STD_MID 7.5
#define STD_INTERVAL 5.0

Stick::Stick() {}

Stick::Stick(float min_duty, float max_duty, float balance_duty, DATA_MODE mode) {}

// 调用这个即可
void Stick::seft_convert(void) {
  seft_standardize();
  switch (mode_) {
    case (NEGATIVE_LOGIC): this->seft_invert();
  }
}

void Stick::seft_invert(void) {
  convert_duty_ = STD_MAX + STD_MIN - std_duty_;
}

void Stick::seft_standardize(void) {
  switch (mode_) {
    case (NEGATIVE_LOGIC): {
      // 归一化到5.0~10.0
      std_duty_ = (now_duty_ - min_duty_) / (max_duty_ - min_duty_)*STD_INTERVAL+STD_MIN;
      break;
    }
  }
}

// 数据接收接口
void Stick::get_now_duty(float now_duty) {
  now_duty_ = now_duty;
  this->seft_convert();
}
