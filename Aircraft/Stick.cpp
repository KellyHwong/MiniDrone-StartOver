#include "Stick.h"

// 这些是标准化之后即理想的值，并不是遥控器真实的值
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
void Stick::data_convert(float nd) {
  this->now_duty_ = nd;
  data_standardize(); // 标准化
  data_invert(); // 正负翻转
  data_unitize(); // 归一化
}

void Stick::data_standardize(void) {
  switch (mode_) {
    // 没有平衡点的情况
    case (NEGATIVE_LOGIC): {
      // 标准化到0.05~0.10
      std_duty_ = (now_duty_-min_duty_)/(max_duty_-min_duty_)*DUTY_INTERVAL + MIN_STICK_DUTY;
      break;
    }
    // 有平衡点的情况
    case (POSITIVE_LOGIC_BALANCED_UNITARY):{
      // 大于平衡点的归一化到0.075~0.10
      if (now_duty_ > balance_duty_) {
        std_duty_ = (now_duty_-balance_duty_)/(max_duty_-balance_duty_)*DUTY_INTERVAL/2 + MID_STICK_DUTY;
      }
      // 小于平衡点的归一化到0.05~0.075
      else {
        std_duty_ = MIN_STICK_DUTY + DUTY_INTERVAL/2*(now_duty_-min_duty_)/(balance_duty_-min_duty_);
      }
      break;
    }
    case (NEGATIVE_LOGIC_BALANCED_UNITARY):{
      // 大于平衡点的归一化到0.075~0.10
      if (now_duty_ > balance_duty_) {
        std_duty_ = (now_duty_-balance_duty_)/(max_duty_-balance_duty_)*DUTY_INTERVAL/2 + MID_STICK_DUTY;
      }
      // 小于平衡点的归一化到0.05~0.075
      else {
        std_duty_ = MIN_STICK_DUTY + DUTY_INTERVAL/2*(now_duty_-min_duty_)/(balance_duty_-min_duty_);
      }
      break;
    }
  }
}

void Stick::data_invert(void) {
  switch (mode_) {
    // 负逻辑的就要反向
    case (NEGATIVE_LOGIC):
    case (NEGATIVE_LOGIC_BALANCED_UNITARY): {
      // 0.05~0.10 -> 0.10~0.05
      inverted_duty_ = MAX_STICK_DUTY + MIN_STICK_DUTY - std_duty_;
      break;
    }
    case (POSITIVE_LOGIC_BALANCED_UNITARY): {
      // 不是do nothing啊！！！是要赋值！否则后面的unitize的inverted_duty_就一直是0！
      inverted_duty_ = std_duty_;
      break;
    }
  }
}

void Stick::data_unitize(void) {
  switch (mode_) {
    // 需要归一化的类型
    case (POSITIVE_LOGIC_BALANCED_UNITARY):{
      // 0.05~0.075 -> -1~0; 0.075~0.10 -> 0~+1
      if (inverted_duty_>=MID_STICK_DUTY) { // > 0.075
        convert_duty_ = (inverted_duty_-MID_STICK_DUTY)/DUTY_INTERVAL*2; // 0~1
      }
      else {
        // 上下貌似是一样的
        convert_duty_ = (inverted_duty_-MID_STICK_DUTY)/DUTY_INTERVAL*2; // -1~0
      }
      break;
    }
    case (NEGATIVE_LOGIC_BALANCED_UNITARY): {
      // 0.05~0.075 -> -1~0; 0.075~0.10 -> 0~+1
      if (inverted_duty_>=MID_STICK_DUTY) { // > 0.075
        convert_duty_ = (inverted_duty_-MID_STICK_DUTY)/DUTY_INTERVAL*2; // 0~1
      }
      else {
        // 上下貌似是一样的
        convert_duty_ = (inverted_duty_-MID_STICK_DUTY)/DUTY_INTERVAL*2; // -1~0
      }
      break;
    }
    // 不需要归一化的类型
    case (NEGATIVE_LOGIC): {
      convert_duty_ = inverted_duty_;
      break;
    }
  }
}
/* 不需要
// 数据接收接口
void Stick::now_duty(float nd) {
  now_duty_ = nd;
}
*/
