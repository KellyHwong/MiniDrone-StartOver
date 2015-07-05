#ifndef STICK_H_
#define STICK_H_

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
  /* 有哪些写哪些，不考虑其他情况 */
  NEGATIVE_LOGIC, // 负逻辑，无平衡点，不需要归一化
  POSITIVE_LOGIC_BALANCED_UNITARY, // 正逻辑，有平衡点，需要归一化为-1~+1
  NEGATIVE_LOGIC_BALANCED_UNITARY // 负逻辑，有平衡点，需要归一化
} DATA_MODE; // 数据模式

class Stick {
 private:
  DATA_MODE mode_;
  float now_duty_; // 现在的占空比，通过调度器接口获得
  float min_duty_; // 操作杆通道最低占空比
  float max_duty_; //
  float balance_duty_; // 不动操作杆时的占空比
  float std_duty_; // 标准化后的占空比
  float inverted_duty_; // 翻转后的占空比
  void data_standardize(void);
  void data_invert(void);
  void data_unitize(void);
 public:
  Stick(float min_duty, float max_duty, float balance_duty, DATA_MODE mode);
  Stick();
  void data_convert(float nd);
  float convert_duty_; // 转换后的占空比（可能有正负）
  void now_duty(float nd); // 进行转换
}; // class Stick

#ifdef __cplusplus
}
#endif

#endif
