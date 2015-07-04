#include "Utilities/Gyroscope.h"

#define STD_MIN 0.05
#define STD_MAX 0.10
#define STD_MID 0.075
#define STD_INTERVAL 0.05

Gyroscope::Gyroscope() {
  min_duty_ = STD_MIN;
  max_duty_ = STD_MAX;
  balance_duty_ = STD_MID;
  mode_ = NEGATIVE_LOGIC;
}

// 调用这个即可
void Gyroscope::Routine(void) {
  seft_standardize();
  switch (mode_) {
    case (NEGATIVE_LOGIC): this->seft_invert();
  }
}

void Gyroscope::seft_invert(void) {
  convert_duty_ = STD_MAX + STD_MIN - std_duty_;
}

void Gyroscope::seft_standardize(void) {
  switch (mode_) {
    case (NEGATIVE_LOGIC): {
      // 归一化到5.0~10.0
      std_duty_ = (now_duty_-min_duty_)/(max_duty_-min_duty_)*STD_INTERVAL + STD_MIN;
      break;
    }
  }
}

// 数据接收接口
void Gyroscope::get_now_duty(float now_duty) {
  now_duty_ = now_duty;
  this->seft_convert();
}
