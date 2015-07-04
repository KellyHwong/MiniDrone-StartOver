#ifndef STICK_H_
#define STICK_H_

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
  NEGATIVE_LOGIC // 负逻辑，反向即可
} DATA_MODE; // 数据模式

class Gyroscope {
 private:

 public:
  // 六轴数据初始化
  Gyroscope(float min_duty, float max_duty, float balance_duty, DATA_MODE mode);
  Gyroscope();
  float convert_duty_; // 转换后的占空比（可能有正负）
  void get_now_duty(float now_duty);
}; // class Gyroscope

#ifdef __cplusplus
}
#endif

#endif
